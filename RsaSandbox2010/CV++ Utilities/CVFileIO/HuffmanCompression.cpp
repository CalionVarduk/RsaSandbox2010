#include "HuffmanCompression.h"

using namespace CVFileIO;

array<Byte>^ HuffmanCompression::compress(array<Byte>^ input)
{
	if(input->Length == 0)
		return gcnew array<Byte>(0);

	HuffmanCodingInfo^ codingInfo = gcnew HuffmanCodingInfo();
	HuffmanTree^ huffmanTree = gcnew HuffmanTree();

	codingInfo->initBytesCount(input);
	huffmanTree->buildCompressionTree(codingInfo);
	codingInfo->parseCompressionTree(huffmanTree);

	UInt32 cInputBitSize;
	Byte inputSizeBits;
	UInt32 cSize = _compressGetSize(codingInfo, input->Length, cInputBitSize, inputSizeBits);

	array<Byte>^ output = gcnew array<Byte>(cSize);
	Random^ rng = gcnew Random();
	UInt32 iByte = codingInfo->parseToOutput(output, rng);

	_compressIncludeInputSize(output, input->Length, inputSizeBits, rng, iByte);
	_compressIncludeInput(input, output, codingInfo, iByte);
	_compressTryRandomLastByte(output, cInputBitSize, rng);

	return output;
}

array<Byte>^ HuffmanCompression::decompress(array<Byte>^ input)
{
	if(input->Length == 0)
		return gcnew array<Byte>(0);

	if(((input[0] + 1) * 3) > input->Length)
		return gcnew array<Byte>(0);

	HuffmanCodingInfo^ codingInfo = gcnew HuffmanCodingInfo();
	HuffmanTree^ huffmanTree = gcnew HuffmanTree();

	try {
		UInt32 iByte = codingInfo->initFromInput(input);
		huffmanTree->buildDecompressionTree(codingInfo);

		UInt32 dSize = _decompressGetSize(input, iByte);
		array<Byte>^ output = gcnew array<Byte>(dSize);
		_decompressInput(input, output, huffmanTree, iByte);		

		return output;
	}
	catch(IndexOutOfRangeException^)
	{ return gcnew array<Byte>(0); }
}

double HuffmanCompression::getEntropy(array<Byte>^ input)
{
	double entropy = 0.0;

	if(input->Length > 0) {
		HuffmanCodingInfo^ info = gcnew HuffmanCodingInfo();
		info->initBytesCount(input);

		double inputLength = input->Length;
		double countByLength;

		for(UInt16 i = 0; i < 256; ++i) {
			if(info->bytesCount[i] > 0) {
				countByLength = info->bytesCount[i] / inputLength;
				entropy += countByLength * Math::Log(countByLength, 2);
			}
		}
	}

	return -entropy;
}

double HuffmanCompression::getAverageCodeLength(array<Byte>^ input)
{
	double avgCodeLength = 0.0;

	if(input->Length > 0) {
		HuffmanCodingInfo^ info = gcnew HuffmanCodingInfo();
		HuffmanTree^ tree = gcnew HuffmanTree();

		info->initBytesCount(input);
		tree->buildCompressionTree(info);
		info->parseCompressionTree(tree);

		double inputLength = input->Length;

		for(UInt16 i = 0; i < 256; ++i)
			if(info->bytesCount[i] > 0)
				avgCodeLength += (info->bytesCount[i] / inputLength) * info->codeLengths[info->treeIndices[i]];
	}

	return avgCodeLength;
}

double HuffmanCompression::getCodeEfficiency(array<Byte>^ input)
{
	double efficiency = 0.0;

	if(input->Length > 0) {
		double avgCodeLength = getAverageCodeLength(input);
		double entropy = getEntropy(input);

		if(entropy > 0)
			efficiency = (entropy / avgCodeLength) * 100.0;
	}

	return efficiency;
}

UInt32 HuffmanCompression::_compressGetSize(HuffmanCodingInfo^ info, UInt32 inputSize, UInt32% out_cInputBitSize, Byte% out_inputSizeBits)
{
	UInt32 cSize;
	info->getRawCompressedSize(cSize, out_cInputBitSize);
	out_inputSizeBits = __compressGetInputSizeBits(inputSize);
	return (cSize + 1) + ((out_inputSizeBits + 7) >> 3);
}

Byte HuffmanCompression::__compressGetInputSizeBits(UInt32 inputSize)
{
	Byte inputSizeBits = 0;

	while(inputSize > 255) {
		inputSize >>= 8;
		inputSizeBits += 8;
	}

	while(inputSize > 0) {
		inputSize >>= 1;
		++inputSizeBits;
	}

	return inputSizeBits;
}

void HuffmanCompression::_compressIncludeInputSize(array<Byte>^ output, UInt32 inputSize, UInt32 inputSizeBits, Random^ rng, UInt32% iByte)
{
	output[iByte++] = (inputSizeBits - 1) | (Byte)(rng->Next(8) << 5);

	while(inputSize > 0) {
		output[iByte++] = (Byte)(inputSize & 255);
		inputSize >>= 8;
	}

	Byte sizeLastBits = inputSizeBits & 7;
	if(sizeLastBits > 0)
		output[iByte - 1] |= (Byte)(rng->Next(1 << (8 - sizeLastBits)) << sizeLastBits);
}

void HuffmanCompression::_compressIncludeInput(array<Byte>^ input, array<Byte>^ output, HuffmanCodingInfo^ info, UInt32 iByte)
{
	Byte iChar;
	UInt64 code;
	Int16 codeLength;
	Byte maxInitBits;
	UInt32 iBit = 0;
	output[output->Length - 1] = 0;

	for(Int32 i = 0; i < input->Length; ++i) {
		iChar = info->treeIndices[input[i]];
		code = info->codes[iChar];
		codeLength = info->codeLengths[iChar];
		maxInitBits = 8 - iBit;
		
		output[iByte] |= (code & ((1 << maxInitBits) - 1)) << iBit;

		if(maxInitBits <= codeLength) {
			++iByte;
			codeLength -= maxInitBits;
			code >>= maxInitBits;

			while(codeLength >= 8) {
				output[iByte++] = code & 255;
				codeLength -= 8;
				code >>= 8;
			}

			iBit = codeLength;
			if(codeLength > 0)
				output[iByte] = code & 255;
		}
		else
			iBit += codeLength;
	}
}

void HuffmanCompression::_compressTryRandomLastByte(array<Byte>^ output, UInt32 cInputBitSize, Random^ rng)
{
	Byte cInputLastBits = cInputBitSize & 7;
	if(cInputLastBits > 0)
		output[output->Length - 1] |= (Byte)(rng->Next(1 << (8 - cInputLastBits)) << cInputLastBits);
}

UInt32 HuffmanCompression::_decompressGetSize(array<Byte>^ input, UInt32% iByte)
{
	Byte outputSizeBits = (input[iByte++] & 31) + 1;
	Byte outputSizeBytes = (outputSizeBits + 7) >> 3;
	UInt32 outputSize = input[iByte++];

	for(UInt16 i = 1; i < outputSizeBytes; ++i)
		outputSize |= input[iByte++] << (i << 3);

	if((outputSizeBits & 7) > 0)
		outputSize &= (1ULL << outputSizeBits) - 1;

	return outputSize;
}

void HuffmanCompression::_decompressInput(array<Byte>^ input, array<Byte>^ output, HuffmanTree^ tree, UInt32 iByte)
{
	UInt16 iCurrent;
	bool currentBit;
	Byte iBit = 0;

	for(Int32 i = 0; i < output->Length; ++i) {
		iCurrent = 0;
		while(tree->items[iCurrent].byte == -1) {
			currentBit = ((input[iByte] & (1 << iBit++)) > 0);
			iCurrent = (currentBit) ? tree->items[iCurrent].iLeft : tree->items[iCurrent].iRight;
			if(iBit == 8) {
				iBit = 0;
				++iByte;
			}
		}
		output[i] = (Byte)tree->items[iCurrent].byte;
	}
}

HuffmanCompression::TreeItem::TreeItem(Int16 _byte)
{
	byte = _byte;
	iLeft = -1;
	iRight = -1;
	iParent = -1;
	visited = false;
}

HuffmanCompression::TreeItem::TreeItem(Int16 _byte, Int32 ileft, Int32 iright, Int32 iparent)
{
	byte = _byte;
	iLeft = ileft;
	iRight = iright;
	iParent = iparent;
	visited = false;
}

HuffmanCompression::TreeRoot::TreeRoot(Int32 _weight, Int32 _index)
{
	weight = _weight;
	index = _index;
}

bool HuffmanCompression::TreeRoot::operator> (TreeRoot lhs, TreeRoot rhs)
{
	return (lhs.weight > rhs.weight);
}

bool HuffmanCompression::TreeRoot::operator< (TreeRoot lhs, TreeRoot rhs)
{
	return (lhs.weight < rhs.weight);
}

HuffmanCompression::HuffmanTree::HuffmanTree()
{
	roots = nullptr;
	items = nullptr;
}

void HuffmanCompression::HuffmanTree::buildCompressionTree(HuffmanCodingInfo^ info)
{
	_initCompressionTree(info);
	UInt16 iTree = info->nUnique;

	while(roots->Count > 1) {
		TreeRoot min1 = roots->extract();
		TreeRoot min2 = roots->extract();

		items[min1.index].iParent = iTree;
		items[min2.index].iParent = iTree;
		items[iTree].iLeft = min1.index;
		items[iTree].iRight = min2.index;

		roots->insert(TreeRoot(min1.weight + min2.weight, iTree++));
	}
}

void HuffmanCompression::HuffmanTree::buildDecompressionTree(HuffmanCodingInfo^ info)
{
	items = gcnew array<TreeItem>((info->nUnique << 1) - 1);
	for(Int32 i = 0; i < items->Length; ++i)
		items[i] = TreeItem::EMPTY;

	Byte iBit;
	Byte codeLengthM1;
	bool currentBit;
	UInt32 iCurrent;
	UInt32 iLeftChild, iRightChild;
	UInt32 iTree = 1;

	for(UInt16 i = 0; i < info->nUnique; ++i) {
		codeLengthM1 = info->codeLengths[i] - 1;
		iCurrent = 0;
		iBit = 0;

		while(iBit < codeLengthM1) {
			currentBit = ((info->codes[i] & (1ULL << iBit++)) > 0);

			if(currentBit) {
				iLeftChild = items[iCurrent].iLeft;
				if(iLeftChild == -1) {
					items[iCurrent].iLeft = iTree;
					iCurrent = iTree++;
				}
				else
					iCurrent = iLeftChild;
			}
			else {
				iRightChild = items[iCurrent].iRight;
				if(iRightChild == -1) {
					items[iCurrent].iRight = iTree;
					iCurrent = iTree++;
				}
				else
					iCurrent = iRightChild;
			}
		}

		currentBit = ((info->codes[i] & (1ULL << iBit)) > 0);

		((currentBit) ?
		items[iCurrent].iLeft :
		items[iCurrent].iRight) = iTree;

		items[iTree++].byte = info->bytes[i];
	}
}

void HuffmanCompression::HuffmanTree::_initCompressionTree(HuffmanCodingInfo^ info)
{
	roots = gcnew CVHeap<TreeRoot>(256);
	items = gcnew array<TreeItem>((info->nUnique << 1) - 1);

	Int16 iItem = 0;
	for(UInt16 i = 0; i < 256; ++i) {
		if(info->bytesCount[i] > 0) {
			items[iItem] = TreeItem((Byte)i);
			roots->insert(TreeRoot(info->bytesCount[i], iItem++));
		}
	}

	while(iItem < items->Length)
		items[iItem++] = TreeItem::EMPTY;
}

HuffmanCompression::HuffmanCodingInfo::HuffmanCodingInfo()
{
	nUnique = 0;
	bytesCount = nullptr;
	bytes = nullptr;
	codeLengths = nullptr;
	codes = nullptr;
	treeIndices = nullptr;
}

void HuffmanCompression::HuffmanCodingInfo::initBytesCount(array<Byte>^ input)
{
	nUnique = 0;
	bytesCount = gcnew array<UInt32>(256);
	for(UInt16 i = 0; i < 256; ++i)
		bytesCount[i] = 0;

	for(Int32 i = 0; i < input->Length; ++i) {
		if(bytesCount[input[i]] == 0)
			++nUnique;
		++bytesCount[input[i]];
	}
}

void HuffmanCompression::HuffmanCodingInfo::parseCompressionTree(HuffmanTree^ tree)
{
	if(tree->items->Length == 1)
		_parseCompressionWithOne(tree);
	else
		_parseCompressionWithMore(tree);
}

void HuffmanCompression::HuffmanCodingInfo::getRawCompressedSize(UInt32% out_cSize, UInt32% out_cInputBitSize)
{
	out_cSize = 1 + (nUnique << 1);
	out_cInputBitSize = 0;
	for(UInt16 i = 0; i < nUnique; ++i) {
		out_cSize += (codeLengths[i] + 7) >> 3;
		out_cInputBitSize += bytesCount[bytes[i]] * codeLengths[i];
	}
	out_cSize += (out_cInputBitSize + 7) >> 3;
}

UInt32 HuffmanCompression::HuffmanCodingInfo::parseToOutput(array<Byte>^ output, Random^ rng)
{
	Byte lengthBytes, lengthLastBits;
	UInt32 iByte = 1;

	output[0] = (Byte)(nUnique - 1);
	for(UInt16 i = 0; i < nUnique; ++i) {
		lengthBytes = (codeLengths[i] + 7) >> 3;
		lengthLastBits = codeLengths[i] & 7;

		output[iByte++] = bytes[i];
		output[iByte++] = (codeLengths[i] - 1) | (Byte)(rng->Next(4) << 6);

		for(UInt16 j = 0; j < lengthBytes; ++j)
			output[iByte++] = (codes[i] >> (j << 3)) & 255;

		if(lengthLastBits > 0)
			output[iByte - 1] |= (Byte)(rng->Next(1 << (8 - lengthLastBits)) << lengthLastBits);
	}

	return iByte;
}

UInt32 HuffmanCompression::HuffmanCodingInfo::initFromInput(array<Byte>^ input)
{
	Byte lengthBytes;
	UInt32 iByte = 1;
	
	nUnique = (UInt16)input[0] + 1;
	bytes = gcnew array<Byte>(nUnique);
	codes = gcnew array<UInt64>(nUnique);
	codeLengths = gcnew array<Byte>(nUnique);

	for(UInt16 i = 0; i < nUnique; ++i) {
		bytes[i] = input[iByte++];
		codeLengths[i] = (input[iByte++] & 63) + 1;

		lengthBytes = (codeLengths[i] + 7) >> 3;
		codes[i] = 0;
		for(UInt16 j = 0; j < lengthBytes; ++j)
			codes[i] |= input[iByte++] << (j << 3);

		if((codeLengths[i] & 7) > 0)
			codes[i] &= (1ULL << codeLengths[i]) - 1;
	}

	return iByte;
}

void HuffmanCompression::HuffmanCodingInfo::_parseCompressionWithOne(HuffmanTree^ tree)
{
	bytes = gcnew array<Byte>(1);
	codes = gcnew array<UInt64>(1);
	codeLengths = gcnew array<Byte>(1);
	treeIndices = gcnew array<Byte>(256);

	bytes[0] = (Byte)tree->items[0].byte;
	codes[0] = 1;
	codeLengths[0] = 1;
	treeIndices[tree->items[0].byte] = 0;
}

void HuffmanCompression::HuffmanCodingInfo::_parseCompressionWithMore(HuffmanTree^ tree)
{
	bytes = gcnew array<Byte>(nUnique);
	codes = gcnew array<UInt64>(nUnique);
	codeLengths = gcnew array<Byte>(nUnique);
	treeIndices = gcnew array<Byte>(256);

	UInt64 code = 0;
	Byte codeLength = 0;
	UInt32 nTraversed = 0;

	UInt32 iCurrent = tree->items->Length - 1;
	UInt32 iLeftChild, iRightChild, iParent;

	while(nTraversed < nUnique) {
		iLeftChild = tree->items[iCurrent].iLeft;

		if(tree->items[iLeftChild].visited == false) {
			if(tree->items[iLeftChild].byte != -1) {
				tree->items[iLeftChild].visited = true;

				bytes[nTraversed] = (Byte)tree->items[iLeftChild].byte;
				codes[nTraversed] = code | (1ULL << codeLength);
				treeIndices[bytes[nTraversed]] = nTraversed;
				codeLengths[nTraversed++] = codeLength + 1;
			}
			else {
				iCurrent = iLeftChild;
				code |= 1ULL << codeLength++;
			}
		}
		else {
			iRightChild = tree->items[iCurrent].iRight;
			iParent = tree->items[iCurrent].iParent;

			if(tree->items[iRightChild].visited == false) {
				if(tree->items[iRightChild].byte != -1) {
					tree->items[iRightChild].visited = true;
					tree->items[iCurrent].visited = true;

					bytes[nTraversed] = (Byte)tree->items[iRightChild].byte;
					codes[nTraversed] = code;
					treeIndices[bytes[nTraversed]] = nTraversed;
					codeLengths[nTraversed++] = codeLength + 1;

					if(codeLength > 0)
						code &= ~(1ULL << --codeLength);
					iCurrent = iParent;
				}
				else {
					iCurrent = iRightChild;
					++codeLength;
				}
			}
			else {
				code &= ~(1ULL << --codeLength);
				tree->items[iCurrent].visited = true;
				iCurrent = iParent;
			}
		}
	}
}
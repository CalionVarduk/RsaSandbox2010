#pragma once

#include "../CVStructures/CVHeap.h"

namespace CVFileIO
{
	using namespace System;
	using namespace CVStructures;

	public ref class HuffmanCompression abstract
	{
		public:
			static array<Byte>^ compress(array<Byte>^ input);
			static array<Byte>^ decompress(array<Byte>^ input);

			static double getEntropy(array<Byte>^ input);
			static double getAverageCodeLength(array<Byte>^ input);
			static double getCodeEfficiency(array<Byte>^ input);

			value class TreeItem {
				public:
					static const TreeItem EMPTY = TreeItem(-1, -1, -1, -1);

					Int32 iParent, iLeft, iRight;
					Int16 byte;
					bool visited;

					TreeItem(Int16 _byte);
					TreeItem(Int16 _byte, Int32 ileft, Int32 iright, Int32 iparent);
			};

			value class TreeRoot {
				public:
					static const TreeRoot EMPTY = TreeRoot(0, -1);

					Int32 index;
					Int32 weight;

					TreeRoot(Int32 _weight, Int32 _index);

					static bool operator> (TreeRoot lhs, TreeRoot rhs);
					static bool operator< (TreeRoot lhs, TreeRoot rhs);
			};

		private:
			ref class HuffmanTree;
			ref class HuffmanCodingInfo;

			ref class HuffmanTree {
				public:
					CVHeap<TreeRoot>^ roots;
					array<TreeItem>^ items;

					HuffmanTree();

					void buildCompressionTree(HuffmanCodingInfo^ info);
					void buildDecompressionTree(HuffmanCodingInfo^ info);

				private:
					void _initCompressionTree(HuffmanCodingInfo^ info);
			};

			ref class HuffmanCodingInfo {
				public:
					UInt16 nUnique;
					array<UInt32>^ bytesCount;
					array<Byte>^ bytes;
					array<Byte>^ codeLengths;
					array<UInt64>^ codes;
					array<Byte>^ treeIndices;

					HuffmanCodingInfo();

					void initBytesCount(array<Byte>^ input);
					void parseCompressionTree(HuffmanTree^ tree);
					void getRawCompressedSize(UInt32% out_cSize, UInt32% out_cInputBitSize);

					UInt32 parseToOutput(array<Byte>^ output, Random^ rng);

					UInt32 initFromInput(array<Byte>^ input);

				private:
					void _parseCompressionWithOne(HuffmanTree^ tree);
					void _parseCompressionWithMore(HuffmanTree^ tree);
			};

			static UInt32 _compressGetSize(HuffmanCodingInfo^ info, UInt32 inputSize, UInt32% out_cInputBitSize, Byte% out_inputSizeBits);
			static Byte __compressGetInputSizeBits(UInt32 inputSize);

			static void _compressIncludeInputSize(array<Byte>^ output, UInt32 inputSize, UInt32 inputSizeBits, Random^ rng, UInt32% iByte);
			static void _compressIncludeInput(array<Byte>^ input, array<Byte>^ output, HuffmanCodingInfo^ info, UInt32 iByte);
			static void _compressTryRandomLastByte(array<Byte>^ output, UInt32 cInputBitSize, Random^ rng);

			static UInt32 _decompressGetSize(array<Byte>^ input, UInt32% iByte);
			static void _decompressInput(array<Byte>^ input, array<Byte>^ output, HuffmanTree^ tree, UInt32 iByte);
	};
}
#pragma once

#include "RSAKey.h"
#include "CVUtilities.h"

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Reflection;
	using namespace System::Collections::Generic;

	using namespace CVFileIO;
	
	public ref class RSAKeyBinaryConverter : public DataConverterBase
	{
		public:
			RSAKeyBinaryConverter() : DataConverterBase()
			{
				n = e = d = p = q = dp = dq = qinv = salt = nullptr;
			}

		protected:
			virtual bool writeDataToBytes() override
			{
				RSAKey^ key = interpretDataAs<RSAKey^>();

				writeUChar(((key->IsFermatSafe) ? 1 : 0) | ((key->IsWienerSafe) ? 2 : 0));
				writeInt(salt->Length);
				writeInt(n->Length);
				writeInt(e->Length);
				writeInt(d->Length);
				writeInt(p->Length);
				writeInt(q->Length);
				writeInt(dp->Length);
				writeInt(dq->Length);
				writeInt(qinv->Length);

				_writeArray(salt);
				_writeArray(n);
				_writeArray(e);
				_writeArray(d);
				_writeArray(p);
				_writeArray(q);
				_writeArray(dp);
				_writeArray(dq);
				_writeArray(qinv);

				_rsaCleanUp();
				return true;
			}

			virtual bool readDataFromBytes() override
			{
				if(BytesLength < 37) return false;

				Byte flags = readUChar();
				Int32 saltLength = readInt();
				Int32 nLength = readInt();
				Int32 eLength = readInt();
				Int32 dLength = readInt();
				Int32 pLength = readInt();
				Int32 qLength = readInt();
				Int32 dpLength = readInt();
				Int32 dqLength = readInt();
				Int32 qinvLength = readInt();

				if(BytesLength != (37 + saltLength + nLength + eLength + dLength + pLength + qLength + dpLength + dqLength + qinvLength)) return false;

				salt = _readArray(saltLength);
				n = _readArray(nLength);
				e = _readArray(eLength);
				d = _readArray(dLength);
				p = _readArray(pLength);
				q = _readArray(qLength);
				dp = _readArray(dpLength);
				dq = _readArray(dqLength);
				qinv = _readArray(qinvLength);

				array<array<Byte>^>^ nums = gcnew array<array<Byte>^>{ salt, n, e, d, p, q, dp, dq, qinv };
				DataObject = (RSAKey^)RSAKey::typeid->GetConstructors(BindingFlags::NonPublic | BindingFlags::Instance)[0]->Invoke(gcnew array<Object^>{ nums, flags });
				_rsaCleanUp();
				return true;
			}

			virtual Int32 dataBinaryLength() override
			{
				RSAKey^ key = interpretDataAs<RSAKey^>();
				if(!key->AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Exponents must be set before converting to binary format.");

				n = key->Modulus.ToByteArray();
				e = key->PublicExponent.ToByteArray();
				d = (array<Byte>^)((array<Byte>^)RSAKey::typeid->GetField("d", BindingFlags::NonPublic | BindingFlags::Instance)->GetValue(key))->Clone();
				p = (array<Byte>^)((array<Byte>^)RSAKey::typeid->GetField("p", BindingFlags::NonPublic | BindingFlags::Instance)->GetValue(key))->Clone();
				q = (array<Byte>^)((array<Byte>^)RSAKey::typeid->GetField("q", BindingFlags::NonPublic | BindingFlags::Instance)->GetValue(key))->Clone();
				dp = (array<Byte>^)((array<Byte>^)RSAKey::typeid->GetField("dp", BindingFlags::NonPublic | BindingFlags::Instance)->GetValue(key))->Clone();
				dq = (array<Byte>^)((array<Byte>^)RSAKey::typeid->GetField("dq", BindingFlags::NonPublic | BindingFlags::Instance)->GetValue(key))->Clone();
				qinv = (array<Byte>^)((array<Byte>^)RSAKey::typeid->GetField("qinv", BindingFlags::NonPublic | BindingFlags::Instance)->GetValue(key))->Clone();

				salt = (key->IsPasswordProtected) ?
						((SHA512Password^)RSAKey::typeid->GetField("_pswrd", BindingFlags::NonPublic | BindingFlags::Instance)->GetValue(key))->Salt :
						gcnew array<Byte>(0);

				Int32 fileSize = 37 + ((key->IsPasswordProtected) ? salt->Length : 0) + 
								 n->Length + e->Length + d->Length + p->Length + q->Length + dp->Length + dq->Length + qinv->Length;

				return fileSize;
			}

		private:
			array<Byte> ^n, ^e, ^d, ^p, ^q, ^dp, ^dq, ^qinv, ^salt;

			void _writeArray(array<Byte>^ data)
			{
				for(Int32 i = 0; i < data->Length; ++i)
					writeUChar(data[i]);
			}

			array<Byte>^ _readArray(Int32 length)
			{
				array<Byte>^ data = gcnew array<Byte>(length);
				for(Int32 i = 0; i < length; ++i)
					data[i] = readUChar();
				return data;
			}

			void _rsaCleanUp()
			{
				Array::Clear(n, 0, n->Length);
				Array::Clear(e, 0, e->Length);
				Array::Clear(d, 0, d->Length);
				Array::Clear(p, 0, p->Length);
				Array::Clear(q, 0, q->Length);
				Array::Clear(dp, 0, dp->Length);
				Array::Clear(dq, 0, dq->Length);
				Array::Clear(qinv, 0, qinv->Length);
				Array::Clear(salt, 0, salt->Length);
				n = e = d = p = q = dp = dq = qinv = salt = nullptr;
			}
	};
}
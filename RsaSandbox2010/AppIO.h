#pragma once

#include "RSAKeyBinaryConverter.h"

namespace RsaSandbox
{
	public ref class AppIO abstract
	{
		public:
			static AppIO()
			{
				keyManager = gcnew FileManager();
				keyManager->setSignature("CVRSA");
				keyManager->setEncryptKey("Czas utworzenia klucza: 2016-04-06 16:37;" + //41
											"Autor algorytmu szyfrujacego i wlasciciel klucza: Lukasz Furlepa (aka CalionVarduk);" + //84
											"Nazwa aplikacji: RSA Sandbox(Licencjat)~;" + //41
											"Aplikacja stworzona na prace dyplomowa, Uniwersytet Slaski - Instytut Matematyki, Katowice", //90
											true);
				keyManager->setCompressState(true);
				keyManager->setEncryptionState(true);

				fileManager = gcnew FileManager();
			}

			static bool saveRsaKey(String^ fileName, RSAKey^ key)
			{
				RSAKeyBinaryConverter^ converter = gcnew RSAKeyBinaryConverter();
				converter->DataObject = key;

				array<Byte>^ data;
				converter->dataToBytes(data);
				keyManager->save(fileName, data);
				return true;
			}

			static bool loadRsaKey(String^ fileName, RSAKey^% key)
			{
				array<Byte>^ data;
				if(keyManager->load(fileName, data)) {
					RSAKeyBinaryConverter^ converter = gcnew RSAKeyBinaryConverter();
					converter->ByteArray = data;

					RSAKey^ newKey;
					if(converter->bytesToData(newKey)) {
						key = newKey;
						return true;
					}
				}
				return false;
			}

			static bool openFile(String^ fileName, array<Byte>^% data)
			{
				return fileManager->load(fileName, data);
			}

			static bool saveFile(String^ fileName, array<Byte>^ data)
			{
				return fileManager->save(fileName, data);
			}

		private:
			static FileManager^ keyManager;
			static FileManager^ fileManager;
	};
}
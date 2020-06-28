#include "pch.h"
#include "usbcomm.h"
#include <mutex>
#include "Logger.h"

namespace usbcomm {
	HANDLE handler;
	bool connected = false;
	std::mutex mutex;
	std::mutex readMutex;
	COMSTAT com;
	DWORD errors;

	bool OpenPort() {
		mutex.lock();
		// TODO - Allow different COM Ports
		handler = CreateFile(L"\\\\.\\COM12", GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (handler == INVALID_HANDLE_VALUE) {
			LOGGER.logError("MACCHINA", "Cannot create handler - error is %d", GetLastError());
			mutex.unlock();
			return false;
		}

		DCB params = { 0x00 };
		if (!GetCommState(handler, &params)) {
			LOGGER.logError("MACCHINA", "Cannot read comm states - error is %d", GetLastError());
			mutex.unlock();
			return false;
		}

		params.BaudRate = CBR_115200;
		params.ByteSize = 8;
		params.StopBits = ONESTOPBIT;
		params.Parity = NOPARITY;
		params.fDtrControl = DTR_CONTROL_DISABLE;

		if (!SetCommState(handler, &params)) {
			LOGGER.logError("MACCHINA", "Cannot set comm states - error is %d", GetLastError());
			mutex.unlock();
			return false;
		}

		PurgeComm(handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
		connected = true;
		mutex.unlock();
		return true;
	}

	void ClosePort() {
		mutex.lock();
		CloseHandle(handler);
		mutex.unlock();
		connected = false;
	}

	bool sendMessage(PCMSG* msg) {
		DWORD written = 0;
		mutex.lock();
		if (!WriteFile(handler, msg, sizeof(struct PCMSG), &written, NULL)) {
			DWORD error = GetLastError();
			LOGGER.logWarn("MACCHINA", "Error writing message! Code %d", (int)error);
			mutex.unlock();
			return false;
		}
		mutex.unlock();
		return true;
	}


	bool pollMessage(PCMSG* msg) {
		DWORD read = 0;
		readMutex.lock();
		ClearCommError(handler, &errors, &com);
		memset(msg, 0x00, sizeof(struct PCMSG));
		if (com.cbInQue >= sizeof(struct PCMSG)) {
			ReadFile(handler, msg, sizeof(struct PCMSG), &read, NULL);
			readMutex.unlock();
			if (read != sizeof(struct PCMSG)) {
				LOGGER.logError("MACCHINA LOG", "Missmatch. Want %lu bytes, got %lu", sizeof(PCMSG), read);
				return false;
			}
			if (msg->cmd_id == CMD_LOG) {
				LOGGER.logInfo("MACCHINA LOG", "%s", msg->args);
				return false;
			}
			return true;
		}
		readMutex.unlock();
		return false;
	}

	bool isConnected() {
		return connected;
	}
}
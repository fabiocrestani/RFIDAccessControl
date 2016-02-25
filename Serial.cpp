///	Serial.cpp - Implementation of the CSerial class
///
///	Copyright (C) 1999-2003 Ramon de Klein (Ramon.de.Klein@ict.nl)
///
/// This library is free software; you can redistribute it and/or
/// modify it under the terms of the GNU Lesser General Public
/// License as published by the Free Software Foundation; either
/// version 2.1 of the License, or (at your option) any later version.
/// 
/// This library is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
/// Lesser General Public License for more details.
/// 
/// You should have received a copy of the GNU Lesser General Public
/// License along with this library; if not, write to the Free Software
/// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

//////////////////////////////////////////////////////////////////////

// Include the standard header files
#include <iostream>
using namespace std;
#define STRICT
#include <crtdbg.h>
#include <tchar.h>
#include <windows.h>

// Include module headerfile
#include "Serial.h"

#pragma warning(disable: 4127) // Disable warning C4127: conditional expression is constant, which
							   // is generated when using the _RPTF and _ASSERTE macros.

#ifdef _DEBUG				   // Enable debug memory manager
	#ifdef THIS_FILE
		#undef THIS_FILE
	#endif

	static const char THIS_FILE[] = __FILE__;
	#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Helper methods

inline void CSerial::CheckRequirements (LPOVERLAPPED lpOverlapped, DWORD dwTimeout) const {
#ifdef SERIAL_NO_OVERLAPPED
	// Check if an overlapped structure has been specified
	if (lpOverlapped || (dwTimeout != INFINITE))
	{
		// Quit application
		::MessageBox(0,_T("Overlapped I/O and time-outs are not supported, when overlapped I/O is disabled."),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
		::DebugBreak();
		::ExitProcess(0xFFFFFFF);
	}
#endif

#ifdef SERIAL_NO_CANCELIO
	// Check if 0 or INFINITE time-out has been specified, because
	// the communication I/O cannot be cancelled.
	if ((dwTimeout != 0) && (dwTimeout != INFINITE))
	{
		// Quit application
		::MessageBox(0,_T("Timeouts are not supported, when SERIAL_NO_CANCELIO is defined"),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
		::DebugBreak();
		::ExitProcess(0xFFFFFFF);
	}
#endif	// SERIAL_NO_CANCELIO

	// Avoid warnings
	(void) dwTimeout;
	(void) lpOverlapped;
}

inline BOOL CSerial::CancelCommIo (void){
#ifdef SERIAL_NO_CANCELIO
	// CancelIo shouldn't have been called at this point
	::DebugBreak();
	return FALSE;
#else
	// Cancel the I/O request
	return ::CancelIo(m_hFile);
#endif	// SERIAL_NO_CANCELIO
}


//////////////////////////////////////////////////////////////////////
// Code

///////////////////////////////////////////////////////////////////////////////////////////////////
/// CSerial (construtor).
/// Prop�sito: <br>
/// Entradas: Nenhuma<br>
/// Sa�das: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
CSerial::CSerial ()
	: m_lLastError(ERROR_SUCCESS)
	, m_hFile(0)
	, m_eEvent(EEventNone)
	, m_dwEventMask(0)
#ifndef SERIAL_NO_OVERLAPPED
	, m_hevtOverlapped(0)
#endif
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CSerial (destrutor).
/// Prop�sito: <br>
/// Entradas: Nenhuma<br>
/// Sa�das: Nenhuma<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
CSerial::~CSerial(){
	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile)
	{
		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::~CSerial - Serial port not closed\n");

		// Close implicitly
		Close();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// CheckPort.
/// Prop�sito: Verifica se a porta � v�lida<br>
/// Entradas: LPCTSTR lpszDevice<br>
/// Sa�das: Nenhuma<br>
///
/// \param lpszDevice
///////////////////////////////////////////////////////////////////////////////////////////////////
CSerial::EPort CSerial::CheckPort (LPCTSTR lpszDevice){
	
	HANDLE hFile = ::CreateFile(lpszDevice,		// Try to open the device
						   GENERIC_READ|GENERIC_WRITE, 
						   0, 
						   0, 
						   OPEN_EXISTING, 
						   0,
						   0);

	if(hFile == INVALID_HANDLE_VALUE){			// Check if we could open the device
		
		switch (::GetLastError()) {				// Display error
			case ERROR_FILE_NOT_FOUND:			// The specified COM-port does not exist
				return EPortNotAvailable;

			case ERROR_ACCESS_DENIED:			// The specified COM-port is in use
				return EPortInUse;

			default:							// Something else is wrong
				return EPortUnknownError;
		}
	}

	::CloseHandle(hFile);						// Close handle
	return EPortAvailable;						// Port is available
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Open.
/// Prop�sito: Abre uma porta serial<br>
/// Entradas: LPCTSTR lpszDevice<br>
/// Sa�das: C�digo de erro se abriu ou n�o a porta<br>
///
/// \param lpszDevice
/// \param dwInQueue
/// \param dwOutQueue
/// \param fOverlapped
///////////////////////////////////////////////////////////////////////////////////////////////////
LONG CSerial::Open (LPCTSTR lpszDevice, DWORD dwInQueue, DWORD dwOutQueue, bool fOverlapped){

	m_lLastError = ERROR_SUCCESS;					// Reset error state
	
	if(m_hFile) {									// Check if the port isn't already opened
		m_lLastError = ERROR_ALREADY_INITIALIZED;
		_RPTF0(_CRT_WARN,"CSerial::Open - Port already opened\n");
		return m_lLastError;
	}

	m_hFile = ::CreateFile(lpszDevice,				// Open the device
						   GENERIC_READ|GENERIC_WRITE,
						   0,
						   0,
						   OPEN_EXISTING,
						   fOverlapped?FILE_FLAG_OVERLAPPED:0,
						   0);

	if(m_hFile == INVALID_HANDLE_VALUE){
		m_hFile = 0;							// Reset file handle

		m_lLastError = ::GetLastError();		// Display error
		_RPTF0(_CRT_WARN, "CSerial::Open - Unable to open port\n");
		return m_lLastError;
	}

#ifndef SERIAL_NO_OVERLAPPED
	_ASSERTE(m_hevtOverlapped == 0);			// We cannot have an event handle yet

	if(fOverlapped){							// Create the event handle for internal overlapped operations (manual reset)
		m_hevtOverlapped = ::CreateEvent(0,true,false,0);
		if(m_hevtOverlapped == 0){
			
			m_lLastError = ::GetLastError();	// Obtain the error information
			_RPTF0(_CRT_WARN,"CSerial::Open - Unable to create event\n");

			::CloseHandle(m_hFile);				// Close the port
			m_hFile = 0;

			return m_lLastError;				// Return the error
		}
	}
#else
	_ASSERTE(!fOverlapped);						// Overlapped flag shouldn't be specified
#endif

	if(dwInQueue || dwOutQueue){				// Setup the COM-port

		// Make sure the queue-sizes are reasonable sized. Win9X systems crash
		// if the input queue-size is zero. Both queues need to be at least
		// 16 bytes large.
		_ASSERTE(dwInQueue >= 16);
		_ASSERTE(dwOutQueue >= 16);

		if(!::SetupComm(m_hFile,dwInQueue,dwOutQueue)){
			
			long lLastError = ::GetLastError(); // Display a warning
			_RPTF0(_CRT_WARN,"CSerial::Open - Unable to setup the COM-port\n");

			Close();							// Close the port

			m_lLastError = lLastError;			// Save last error from SetupComm
			return m_lLastError;	
		}
	}

	SetMask();									// Setup the default communication mask
	SetupReadTimeouts(EReadTimeoutNonblocking); // Non-blocking reads is default

 	COMMCONFIG commConfig = {0};				// Setup the device for default settings
	DWORD dwSize = sizeof(commConfig);
	commConfig.dwSize = dwSize;
	if(::GetDefaultCommConfig(lpszDevice,&commConfig,&dwSize)) {
		// Set the default communication configuration
		if(!::SetCommConfig(m_hFile,&commConfig,dwSize)){
			// Display a warning
			_RPTF0(_CRT_WARN,"CSerial::Open - Unable to set default communication configuration.\n");
		}
	}
	else{
		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::Open - Unable to obtain default communication configuration.\n");
	}
	
	return m_lLastError;						// Return successful
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Close.
/// Prop�sito: Fecha uma porta serial<br>
/// Entradas: Nenhuma<br>
/// Sa�das: C�digo de erro se fechou ou n�o a porta<br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LONG CSerial::Close(void){
	
	m_lLastError = ERROR_SUCCESS;				// Reset error state
	
	if(m_hFile == 0){							// If the device is already closed, then we don't need to do anything.
		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::Close - Method called when device is not open\n");
		return m_lLastError;
	}

#ifndef SERIAL_NO_OVERLAPPED
	if(m_hevtOverlapped){						// Free event handle
		::CloseHandle(m_hevtOverlapped);
		m_hevtOverlapped = 0;
	}
#endif

	::CloseHandle(m_hFile);						// Close COM port
	m_hFile = 0;
	return m_lLastError;						// Return successful
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Setup.
/// Prop�sito: Configura os par�metros da porta serial<br>
/// Entradas: EBaudrate eBaudrate, EDataBits eDataBits, EParity eParity, EStopBits eStopBits<br>
/// Sa�das: C�digo de erro se fechou ou n�o a porta<br>
///
/// \param eBaudrate Baudrate da comunica��o serial
/// \param eDataBits Bits de dados da comunica��o serial
/// \param eParity Bits de paridade da comunica��o serial
/// \param eStopBits Bits de stop da comunica��o serial
///////////////////////////////////////////////////////////////////////////////////////////////////
LONG CSerial::Setup (EBaudrate eBaudrate, EDataBits eDataBits, EParity eParity, EStopBits eStopBits){
	
	m_lLastError = ERROR_SUCCESS;						// Reset error state

	if(m_hFile == 0){									// Check if the device is open
		m_lLastError = ERROR_INVALID_HANDLE;			// Set the internal error code
														// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Setup - Device is not opened\n");
		return m_lLastError;
	}

	CDCB dcb;											// Obtain the DCB structure for the device
	if(!::GetCommState(m_hFile,&dcb)){
		m_lLastError = ::	GetLastError();				// Obtain the error code
														// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::Setup - Unable to obtain DCB information\n");
		return m_lLastError;
	}

	dcb.BaudRate = DWORD(eBaudrate);					// Set the new data
	dcb.ByteSize = BYTE(eDataBits);
	dcb.Parity   = BYTE(eParity);
	dcb.StopBits = BYTE(eStopBits);
	dcb.fParity  = (eParity != EParNone);				// Determine if parity is used

	if(!::SetCommState(m_hFile,&dcb)){					// Set the new DCB structure
		m_lLastError = ::GetLastError();				// Obtain the error code
														// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::Setup - Unable to set DCB information\n");
		return m_lLastError;
	}
	
	return m_lLastError;								// Return successful
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetEventChar.
/// Prop�sito: <br>
/// Entradas: BYTE bEventChar, bool fAdjustMask<br>
/// Sa�das: <br>
///
///////////////////////////////////////////////////////////////////////////////////////////////////
LONG CSerial::SetEventChar (BYTE bEventChar, bool fAdjustMask){
	
	m_lLastError = ERROR_SUCCESS;					// Reset error state

	if(m_hFile == 0){								// Check if the device is open
		m_lLastError = ERROR_INVALID_HANDLE;		// Set the internal error code
													// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::SetEventChar - Device is not opened\n");
		return m_lLastError;
	}
	
	CDCB dcb;										// Obtain the DCB structure for the device
	if(!::GetCommState(m_hFile,&dcb)){
		m_lLastError = ::GetLastError();			// Obtain the error code
													// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetEventChar - Unable to obtain DCB information\n");
		return m_lLastError;
	}

	dcb.EvtChar = char(bEventChar);					// Set the new event character

	if(fAdjustMask){								// Adjust the event mask, to make sure the event will be received

		SetMask(GetEventMask() | EEventRcvEv);		// Enable 'receive event character' event.  Note that this
													// will generate an EEventNone if there is an asynchronous
													// WaitCommEvent pending.
	}

	if(!::SetCommState(m_hFile,&dcb)){				// Set the new DCB structure
		m_lLastError = ::GetLastError();			// Obtain the error code
													// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetEventChar - Unable to set DCB information\n");
		return m_lLastError;
	}

	return m_lLastError;							// Return successful
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// SetMask.
/// Prop�sito: <br>
/// Entradas: DWORD dwEventMask<br>
/// Sa�das: <br>
///
/// \param [in] dwEventMask
///////////////////////////////////////////////////////////////////////////////////////////////////
LONG CSerial::SetMask (DWORD dwEventMask){
	m_lLastError = ERROR_SUCCESS;					// Reset error state
	
	if(m_hFile == 0){								// Check if the device is open
		m_lLastError = ERROR_INVALID_HANDLE;		// Set the internal error code
													// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::SetMask - Device is not opened\n");
		return m_lLastError;
	}

	// Set the new mask. Note that this will generate an EEventNone
	// if there is an asynchronous WaitCommEvent pending.
	if(!::SetCommMask(m_hFile,dwEventMask)){
		m_lLastError = ::GetLastError();			// Obtain the error code
													// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetMask - Unable to set event mask\n");
		return m_lLastError;
	}

	m_dwEventMask = dwEventMask;					// Save event mask and return successful
	return m_lLastError;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// WaitEvent.
/// Prop�sito: <br>
/// Entradas: LPOVERLAPPED lpOverlapped, DWORD dwTimeout<br>
/// Sa�das: <br>
///
/// \param [in] lpOverlapped
/// \param [in] dwTimeout
///////////////////////////////////////////////////////////////////////////////////////////////////
LONG CSerial::WaitEvent (LPOVERLAPPED lpOverlapped, DWORD dwTimeout){
	
	CheckRequirements(lpOverlapped,dwTimeout);		// Check if time-outs are supported
	m_lLastError = ERROR_SUCCESS;					// Reset error state

	if(m_hFile == 0){								// Check if the device is open
		m_lLastError = ERROR_INVALID_HANDLE;		// Set the internal error code
													// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::WaitEvent - Device is not opened\n");
		return m_lLastError;
	}

#ifndef SERIAL_NO_OVERLAPPED
	// Check if an overlapped structure has been specified
	if(!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE))){
		
		m_lLastError = ERROR_INVALID_FUNCTION;		// Set the internal error code
													// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::WaitEvent - Overlapped I/O is disabled, specified parameters are illegal.\n");
		return m_lLastError;
	}
	
	OVERLAPPED ovInternal;							// Wait for the event to happen
	if(!lpOverlapped && m_hevtOverlapped){
		memset(&ovInternal,0,sizeof(ovInternal));	// Setup our own overlapped structure
		ovInternal.hEvent = m_hevtOverlapped;
		lpOverlapped = &ovInternal;					// Use our internal overlapped structure
	}

	// Make sure the overlapped structure isn't busy
	_ASSERTE(!m_hevtOverlapped || HasOverlappedIoCompleted(lpOverlapped));

	// Wait for the COM event
	if(!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),lpOverlapped)){
		
		long lLastError = ::GetLastError();			// Set the internal error code

		if(lLastError != ERROR_IO_PENDING){			// Overlapped operation in progress is not an actual error
			m_lLastError = lLastError;				// Save the error
													// Issue an error and quit
			_RPTF0(_CRT_WARN,"CSerial::WaitEvent - Unable to wait for COM event\n");
			return m_lLastError;
		}
		
		if(lpOverlapped == &ovInternal){			// We need to block if the client didn't specify an overlapped structure

			// Wait for the overlapped operation to complete
			switch(::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout)){
				case WAIT_OBJECT_0:					// The overlapped operation has completed
					break;

				case WAIT_TIMEOUT:					
					CancelCommIo();					// Cancel the I/O operation
					m_lLastError = ERROR_TIMEOUT;	// The operation timed out. Set the internal error code and quit
					return m_lLastError;

				default:
					m_lLastError = ::GetLastError();// Set the internal error code
													// Issue an error and quit
					_RPTF0(_CRT_WARN,"CSerial::WaitEvent - Unable to wait until COM event has arrived\n");
					return m_lLastError;
			}
		}
	}
	else{
		if(lpOverlapped)							// The operation completed immediatly. 
			::SetEvent(lpOverlapped->hEvent);		// Just to be sure we'll set the overlapped structure's event handle.
	}
#else
	// Wait for the COM event
	if(!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),0)){
		m_lLastError = ::GetLastError();			// Set the internal error code
													// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::WaitEvent - Unable to wait for COM event\n");
		return m_lLastError;
	}
#endif

	return m_lLastError;							// Return successfully
}


LONG CSerial::SetupHandshaking (EHandshake eHandshake)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::SetupHandshaking - Device is not opened\n");
		return m_lLastError;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetupHandshaking - Unable to obtain DCB information\n");
		return m_lLastError;
	}

	// Set the handshaking flags
	switch (eHandshake)
	{
	case EHandshakeOff:
		dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
		dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
		break;

	case EHandshakeHardware:
		dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
		dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
		break;

	case EHandshakeSoftware:
		dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
		dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
		dcb.fOutX = true;							// Enable XON/XOFF for transmission
		dcb.fInX = true;							// Enable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
		break;

	default:
		// This shouldn't be possible
		_ASSERTE(false);
		m_lLastError = E_INVALIDARG;
		return m_lLastError;
	}

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetupHandshaking - Unable to set DCB information\n");
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial::SetupReadTimeouts (EReadTimeout eReadTimeout)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::SetupReadTimeouts - Device is not opened\n");
		return m_lLastError;
	}

	// Determine the time-outs
	COMMTIMEOUTS cto;
	if (!::GetCommTimeouts(m_hFile,&cto))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetupReadTimeouts - Unable to obtain timeout information\n");
		return m_lLastError;
	}

	// Set the new timeouts
	switch (eReadTimeout)
	{
	case EReadTimeoutBlocking:
		cto.ReadIntervalTimeout = 0;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	case EReadTimeoutNonblocking:
		cto.ReadIntervalTimeout = MAXDWORD;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	default:
		// This shouldn't be possible
		_ASSERTE(false);
		m_lLastError = E_INVALIDARG;
		return m_lLastError;
	}

	// Set the new DCB structure
	if (!::SetCommTimeouts(m_hFile,&cto))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetupReadTimeouts - Unable to set timeout information\n");
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

CSerial::EBaudrate CSerial::GetBaudrate (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetBaudrate - Device is not opened\n");
		return EBaudUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetBaudrate - Unable to obtain DCB information\n");
		return EBaudUnknown;
	}

	// Return the appropriate baudrate
	return EBaudrate(dcb.BaudRate);
}

CSerial::EDataBits CSerial::GetDataBits (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetDataBits - Device is not opened\n");
		return EDataUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetDataBits - Unable to obtain DCB information\n");
		return EDataUnknown;
	}

	// Return the appropriate bytesize
	return EDataBits(dcb.ByteSize);
}

CSerial::EParity CSerial::GetParity (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetParity - Device is not opened\n");
		return EParUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetParity - Unable to obtain DCB information\n");
		return EParUnknown;
	}

	// Check if parity is used
	if (!dcb.fParity)
	{
		// No parity
		return EParNone;
	}

	// Return the appropriate parity setting
	return EParity(dcb.Parity);
}

CSerial::EStopBits CSerial::GetStopBits (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetStopBits - Device is not opened\n");
		return EStopUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetStopBits - Unable to obtain DCB information\n");
		return EStopUnknown;
	}

	// Return the appropriate stopbits
	return EStopBits(dcb.StopBits);
}

DWORD CSerial::GetEventMask (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetEventMask - Device is not opened\n");
		return 0;
	}

	// Return the event mask
	return m_dwEventMask;
}

BYTE CSerial::GetEventChar (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetEventChar - Device is not opened\n");
		return 0;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetEventChar - Unable to obtain DCB information\n");
		return 0;
	}

	// Set the new event character
	return BYTE(dcb.EvtChar);
}

CSerial::EHandshake CSerial::GetHandshaking (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetHandshaking - Device is not opened\n");
		return EHandshakeUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetHandshaking - Unable to obtain DCB information\n");
		return EHandshakeUnknown;
	}

	// Check if hardware handshaking is being used
	if ((dcb.fDtrControl == DTR_CONTROL_HANDSHAKE) && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
		return EHandshakeHardware;

	// Check if software handshaking is being used
	if (dcb.fOutX && dcb.fInX)
		return EHandshakeSoftware;

	// No handshaking is being used
	return EHandshakeOff;
}

LONG CSerial::Write (const void* pData, size_t iLen, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Check if time-outs are supported
	CheckRequirements(lpOverlapped,dwTimeout);

	// Overlapped operation should specify the pdwWritten variable
	_ASSERTE(!lpOverlapped || pdwWritten);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Use our own variable for read count
	DWORD dwWritten;
	if (pdwWritten == 0)
	{
		pdwWritten = &dwWritten;
	}

	// Reset the number of bytes written
	*pdwWritten = 0;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Write - Device is not opened\n");
		return m_lLastError;
	}

#ifndef SERIAL_NO_OVERLAPPED

	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Write - Overlapped I/O is disabled, specified parameters are illegal.\n");
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (!lpOverlapped && m_hevtOverlapped)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy
	_ASSERTE(!m_hevtOverlapped || HasOverlappedIoCompleted(lpOverlapped));

	// Write the data
	if (!::WriteFile(m_hFile,pData,iLen,pdwWritten,lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			_RPTF0(_CRT_WARN,"CSerial::Write - Unable to write the data\n");
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwWritten,FALSE))
				{
					// Set the internal error code
					m_lLastError = ::GetLastError();

					_RPTF0(_CRT_WARN,"CSerial::Write - Overlapped completed without result\n");
					return m_lLastError;
				}
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelCommIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				_RPTF0(_CRT_WARN,"CSerial::Write - Unable to wait until data has been sent\n");
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		if (lpOverlapped)
			::SetEvent(lpOverlapped->hEvent);
	}

#else

	// Write the data
	if (!::WriteFile(m_hFile,pData,iLen,pdwWritten,0))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Write - Unable to write the data\n");
		return m_lLastError;
	}

#endif

	// Return successfully
	return m_lLastError;
}

LONG CSerial::Write (LPCSTR pString, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Check if time-outs are supported
	CheckRequirements(lpOverlapped,dwTimeout);

	// Determine the length of the string
	return Write(pString,strlen(pString),pdwWritten,lpOverlapped,dwTimeout);
}

LONG CSerial::Read (void* pData, size_t iLen, DWORD* pdwRead, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Check if time-outs are supported
	CheckRequirements(lpOverlapped,dwTimeout);

	// Overlapped operation should specify the pdwRead variable
	_ASSERTE(!lpOverlapped || pdwRead);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Use our own variable for read count
	DWORD dwRead;
	if (pdwRead == 0)
	{
		pdwRead = &dwRead;
	}

	// Reset the number of bytes read
	*pdwRead = 0;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Read - Device is not opened\n");
		return m_lLastError;
	}

#ifdef _DEBUG
	// The debug version fills the entire data structure with
	// 0xDC bytes, to catch buffer errors as soon as possible.
	memset(pData,0xDC,iLen);
#endif

#ifndef SERIAL_NO_OVERLAPPED

	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Read - Overlapped I/O is disabled, specified parameters are illegal.\n");
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy
	_ASSERTE(!m_hevtOverlapped || HasOverlappedIoCompleted(lpOverlapped));
	
	// Read the data
	if (!::ReadFile(m_hFile,pData,iLen,pdwRead,lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			_RPTF0(_CRT_WARN,"CSerial::Read - Unable to read the data\n");
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwRead,FALSE))
				{
					// Set the internal error code
					m_lLastError = ::GetLastError();

					_RPTF0(_CRT_WARN,"CSerial::Read - Overlapped completed without result\n");
					return m_lLastError;
				}
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelCommIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				_RPTF0(_CRT_WARN,"CSerial::Read - Unable to wait until data has been read\n");
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		if (lpOverlapped)
			::SetEvent(lpOverlapped->hEvent);
	}

#else
	
	// Read the data
	if (!::ReadFile(m_hFile,pData,iLen,pdwRead,0))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Read - Unable to read the data\n");
		return m_lLastError;
	}

#endif

	// Return successfully
	return m_lLastError;
}

LONG CSerial::Purge()
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Purge - Device is not opened\n");
		return m_lLastError;
	}

	if (!::PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();
		_RPTF0(_CRT_WARN,"CSerial::Purge - Overlapped completed without result\n");
	}
	
	// Return successfully
	return m_lLastError;
}

LONG CSerial::Break (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Break - Device is not opened\n");
		return m_lLastError;
	}

    // Set the RS-232 port in break mode for a little while
    ::SetCommBreak(m_hFile);
    ::Sleep(100);
    ::ClearCommBreak(m_hFile);

	// Return successfully
	return m_lLastError;
}

CSerial::EEvent CSerial::GetEventType (void)
{
#ifdef _DEBUG
	// Check if the event is within the mask
	if ((m_eEvent & m_dwEventMask) == 0)
		_RPTF2(_CRT_WARN,"CSerial::GetEventType - Event %08Xh not within mask %08Xh.\n", m_eEvent, m_dwEventMask);
#endif

	// Obtain the event (mask unwanted events out)
	EEvent eEvent = EEvent(m_eEvent & m_dwEventMask);

	// Reset internal event type
	m_eEvent = EEventNone;

	// Return the current cause
	return eEvent;
}

CSerial::EError CSerial::GetError (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetError - Device is not opened\n");
		return EErrorUnknown;
	}

	// Obtain COM status
	DWORD dwErrors = 0;
	if (!::ClearCommError(m_hFile,&dwErrors,0))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetError - Unable to obtain COM status\n");
		return EErrorUnknown;
	}

	// Return the error
	return EError(dwErrors);
}

bool CSerial::GetCTS (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetCTS - Unable to obtain the modem status\n");
		return false;
	}

	// Determine if CTS is on
	return (dwModemStat & MS_CTS_ON) != 0;
}

bool CSerial::GetDSR (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetDSR - Unable to obtain the modem status\n");
		return false;
	}

	// Determine if DSR is on
	return (dwModemStat & MS_DSR_ON) != 0;
}

bool CSerial::GetRing (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetRing - Unable to obtain the modem status");
		return false;
	}

	// Determine if Ring is on
	return (dwModemStat & MS_RING_ON) != 0;
}

bool CSerial::GetRLSD (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetRLSD - Unable to obtain the modem status");
		return false;
	}

	// Determine if RLSD is on
	return (dwModemStat & MS_RLSD_ON) != 0;
}

// --------------------------------------------------------------------------------------------------
// Meus m�todos -------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////////
/// VerificaSeAbriu.
/// Prop�sito: Verifica o c�digo de retorno da tentativa de abrir a porta e trata um poss�vel erro
/// Entradas: O c�digo de erro lLastError e o nome da porta nomePortaSerial
/// Sa�das: Retorna true se abriu ou false caso contr�rio
///
/// \param [in] lLastError c�digo do erro proveniente da abertura da porta
/// \param [in] nomePortaSerial TCHAR com o nome da porta, para exibir na mensagem de erro
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CSerial::VerificaSeAbriu(LONG lLastError, const TCHAR nomePortaSerial[]){

	if(lLastError != ERROR_SUCCESS){
		cout << " Serial: N�o foi poss�vel abrir a porta ";
		printf("COM%c\n", nomePortaSerial[3]);
		system("pause");		
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// VerificaSeConfigurou.
/// Prop�sito: Verifica o c�digo de retorno da tentativa de configurar a porta e trata o erro
/// Entradas: O c�digo de erro lLastError
/// Sa�das: Retorna true se configurou ou false caso contr�rio
///
/// \param [in] lLastError c�digo do erro proveniente da configura��o da porta
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CSerial::VerificaSeConfigurou(LONG lLastError){
	if (lLastError != ERROR_SUCCESS){
		cout << " N�o foi poss�vel configurar a porta COM5";
		system("pause");		
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// MostraConectado.
/// Prop�sito: Exibe na tela o status da conex�o (conectado ou n�o)
/// Entradas: Nenhuma
/// Sa�das: Apenas apresenta o status no console
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CSerial::MostraConectado(){
	cout << " SERIAL: Conectado!" << endl;
}
﻿#include <Windows.h>
#include <iostream>
#include <sstream>
#include <dbghelp.h>
#include "Boring32.Tests.h"
#include "../Boring32/include/Boring32.hpp"

#include "pathcch.h"
#pragma comment(lib, "Pathcch.lib")

void print_exception_info(const std::exception& e)
{
	std::wcout << e.what() << std::endl;
	try 
	{
		rethrow_if_nested(e);
	}
	catch (const std::exception& ne)
	{
		print_exception_info(ne);
	}
}

void TestException()
{
	try
	{
		throw std::logic_error("first");
	}
	catch (const std::exception& ex)
	{
		try
		{
			std::throw_with_nested(std::logic_error("second"));
		}
		catch (const std::exception& ex2)
		{
			print_exception_info(ex);
		}
	}
}

void TestWaitableTime(int64_t relativeMillis)
{
	Boring32::Async::WaitableTimer timer1(L"WaitableTimer", false, false);
	Boring32::Async::WaitableTimer timer2(L"WaitableTimer", false, false, SYNCHRONIZE);
	std::wcout << L"Timer set for " << relativeMillis << L" from now" << std::endl;
	timer1.SetTimerInMillis(-relativeMillis, relativeMillis, nullptr, nullptr);
	timer1.WaitOnTimer(INFINITE);
	timer1.CancelTimer();
}

void TestSemaphore()
{
	Boring32::Async::Semaphore semaphore(L"Sem", false, 10, 10);
	semaphore.Acquire(2, INFINITE);
	if (semaphore.GetCurrentCount() != 8)
		throw std::runtime_error("Invalid error count");
	semaphore.Release(2);
	if (semaphore.GetCurrentCount() != 10)
		throw std::runtime_error("Invalid error count");
}

void TestMutex()
{
	Boring32::Async::Mutex m1(L"HelloMutex", false, false);
	m1.Lock(1000);
	m1.Unlock();

	Boring32::Async::Mutex m2(m1);
	Boring32::Async::Mutex m3(false, false);

	m2 = m3;
	m2 = Boring32::Async::Mutex(L"Assignment", false, false);
}

void TestConversions()
{
	std::wstring wstrTest = L"Test1";
	std::string strTest = Boring32::Strings::ConvertWStringToString(wstrTest);
	std::wstring wstrTest2 = Boring32::Strings::ConvertStringToWString(strTest);

	if (wstrTest != wstrTest2)
		throw std::runtime_error("Strings do not match");
}

void TestMemoryMappedFile()
{
	Boring32::Async::MemoryMappedFile m1(L"HelloMmf1", 1000, false);
	Boring32::Async::MemoryMappedFile m2(m1);
	Boring32::Async::MemoryMappedFile m3(L"HelloMmf3", 1000, false);
	m2 = m3;
}

void TestLibraryLoad()
{
	Boring32::Library::DynamicLinkLibrary d(L"wlanapi.dll");
	//void* mainFunc = d.Resolve(L"GetMainInterface");
	//if (mainFunc == nullptr)
	//	throw new std::runtime_error("Could not resolve function");
}

void TestThreadSafeVector()
{
	Boring32::Async::ThreadSafeVector<int> testVector;
	testVector.Add(1);
	testVector.Add(2);
	testVector.Add(1);
	testVector.Add(1);
	testVector.Add(5);
	// Erase all 1s
	testVector.EraseMultiple(
		[](const int& val) -> bool
		{
			return val == 1;
		});
	if(testVector.Size() != 2)
		throw std::runtime_error("Unexpected size of vector");
	if (testVector.CopyOfElementAt(0) != 2)
		throw std::runtime_error("Unexpected element at index 0");
}

void TestAnonPipes()
{
	std::wstring msg1(L"message1");
	std::wstring msg2(L"message2");
	Boring32::Async::AnonymousPipe pipe(true, 512, L"||");
	pipe.DelimitedWrite(msg1);
	pipe.DelimitedWrite(msg2);

	std::vector<std::wstring> response = pipe.DelimitedRead();
	if(response.size() != 2)
		throw std::runtime_error("Unexpected number of tokens");
	std::wcout << response[0] << L" " << response[1] << std::endl;
	if (msg1 != response[0] || msg2 != response[1])
		throw std::runtime_error("Failed to match input to output");
}

void TestProcessBlockingNamedPipe()
{
	std::wstring directory;
	directory.resize(2048);
	GetModuleFileName(nullptr, &directory[0], directory.size());
	PathCchRemoveFileSpec(&directory[0], directory.size());
	directory.erase(std::find(directory.begin(), directory.end(), '\0'), directory.end());
	std::wstring filePath = directory + L"\\TestProcess.exe";

	Boring32::Async::Job job(false);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli{ 0 };
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	job.SetInformation(jeli);
	std::wstringstream ss;
	ss	<< "TestProcess.exe"
		<< L" 1";
	Boring32::Async::Process testProcess(filePath, ss.str(), directory, true);
	testProcess.Start();
	job.AssignProcessToThisJob(testProcess.GetProcessHandle());

	Boring32::Async::BlockingNamedPipeServer pipeServer(
		L"\\\\.\\pipe\\mynamedpipe", 
		200, 
		5, 
		L"", 
		false,
		true
	);
	pipeServer.Connect();
	pipeServer.Write(L"HAHA!");
	pipeServer.Write(L"HAHA2!");
	WaitForSingleObject(testProcess.GetProcessHandle(), INFINITE);
	pipeServer.Disconnect();
}

void TestProcessOverlappedNamedPipe()
{
	std::wstring directory;
	directory.resize(2048);
	GetModuleFileName(nullptr, &directory[0], directory.size());
	PathCchRemoveFileSpec(&directory[0], directory.size());
	directory.erase(std::find(directory.begin(), directory.end(), '\0'), directory.end());
	std::wstring filePath = directory + L"\\TestProcess.exe";

	Boring32::Async::OverlappedNamedPipeServer serverPipe(
		L"\\\\.\\pipe\\mynamedpipe",
		1024,
		5,
		L"",
		false,
		true
	);
	auto oio = serverPipe.Connect();

	Boring32::Async::Job job(false);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli{ 0 };
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	job.SetInformation(jeli);
	std::wstringstream ss;
	ss	<< "TestProcess.exe"
		<< L" 2";
	Boring32::Async::Process testProcess(filePath, ss.str(), directory, true);
	testProcess.Start();
	job.AssignProcessToThisJob(testProcess.GetProcessHandle());

	oio.WaitForCompletion(INFINITE);
	oio = serverPipe.Write(L"HAHA!");
	WaitForSingleObject(testProcess.GetProcessHandle(), INFINITE);
	auto oio2 = serverPipe.Read(1024);
	oio2.WaitForCompletion(INFINITE);
	std::wcout << oio2.IoBuffer << std::endl;
	oio2 = serverPipe.Read(1024);
	oio2.WaitForCompletion(INFINITE);
	std::wcout << oio2.IoBuffer << std::endl;
	int i = 0;
}

void TestProcessAnonPipe()
{
	std::wstring directory;
	directory.resize(2048);
	GetModuleFileName(nullptr, &directory[0], directory.size());
	PathCchRemoveFileSpec(&directory[0], directory.size());
	directory.erase(std::find(directory.begin(), directory.end(), '\0'), directory.end());
	std::wstring filePath = directory + L"\\TestProcess.exe";

	Boring32::Async::Event evt(true, true, false, L"TestEvent");
	Boring32::Async::AnonymousPipe childWrite;
	Boring32::Async::AnonymousPipe childRead;
	childRead = Boring32::Async::AnonymousPipe(true, 2048, L"||");
	childWrite = Boring32::Async::AnonymousPipe(true, 2048, L"||");
	std::wstringstream ss;
	ss << "TestProcess.exe"
		<< L" 3 "
		<< (int)childWrite.GetWrite()
		<< L" "
		<< (int)childRead.GetRead();
	Boring32::Async::Process testProcess(filePath, ss.str(), directory, true);

	Boring32::Async::Job job(false);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli{ 0 };
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	job.SetInformation(jeli);
	testProcess.Start();
	job.AssignProcessToThisJob(testProcess.GetProcessHandle());

	childRead.DelimitedWrite(L"Hello from parent!");
	Sleep(500);

	std::wcout
		<< childWrite.Read() 
		<< std::endl;
	evt.Signal();
	WaitForSingleObject(testProcess.GetProcessHandle(), INFINITE);
}

void TestCompression()
{
	Boring32::Compression::Compressor compressor(Boring32::Compression::CompressionType::MSZIP);
	Boring32::Compression::Decompressor decompressor(Boring32::Compression::CompressionType::MSZIP);
}

void TestTimerQueues()
{
	std::wcout << L"Testing timer queues..." << std::endl;
	Boring32::Async::Event eventToWaitOn(false, true, false, L"");
	Boring32::Async::TimerQueue timerQueue;
	Boring32::Async::TimerQueueTimer timerQueueTimer(
		timerQueue.GetHandle(),
		3000,
		0,
		0,
		[](void* lpParam, BOOLEAN TimerOrWaitFired) -> void 
		{
			Boring32::Async::Event* eventObj = (Boring32::Async::Event*)lpParam;
			eventObj->Signal();
		},
		&eventToWaitOn
	);

	eventToWaitOn.WaitOnEvent(INFINITE, true);
	std::wcout << L"Timer Queue test OK" << std::endl;
}

int main(int argc, char** args)
{
	/*try
	{
		Boring32::Async::OverlappedNamedPipeServer server(L"A", 512, 1, L"", false, true);
		auto a = server.Connect();
		Boring32::Async::OverlappedNamedPipeClient client(L"A");
		client.Connect(0);
		client.Close();
		auto b = server.Read(0);
		b.WaitForCompletion(INFINITE);
		std::wcout << b.IsSuccessful() << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::wcout << ex.what() << std::endl;
	}*/

	PROCESS_MEMORY_EXHAUSTION_INFO pmei{ 0 };
	pmei.Version = PME_CURRENT_VERSION;
	pmei.Type = PMETypeFailFastOnCommitFailure;
	pmei.Value = PME_FAILFAST_ON_COMMIT_FAIL_ENABLE;
	bool succeeded = SetProcessInformation(
		GetCurrentProcess(),
		ProcessMemoryExhaustionInfo,
		&pmei,
		sizeof(pmei)
	);

	try
	{
		Boring32::Async::ThreadPool tp1(1, 10);

		// https://spys.one/en/https-ssl-proxy/
		//Test proxy: L"185.20.224.239:3128"
		//Test proxy: L"45.76.52.195:8081"
		//Test proxy: L"36.67.96.217:3128"
		//Test proxy: L"36.82.95.67:3128"
		//Test proxy: 125.164.86.89:3128
		/*Boring32::WinHttp::HttpWebClient webClient(
			L"test-ua", 
			L"google.com", 
			L"36.90.12.80:8181", 
			443, 
			false,
			{L"text/html"},
			{L""}
		);
		webClient.Connect();
		Boring32::WinHttp::HttpRequestResult result = webClient.Get(L"/");
		std::wcout << result.StatusCode << std::endl;
		std::wcout << result.ResponseBody.c_str() << std::endl;
		*/
		Boring32::WinHttp::WebSockets::WebSocket socket(
			Boring32::WinHttp::WebSockets::WebSocketSettings{
				.Server = L"echo.websocket.org",
				.Port = 443,
				.IgnoreSslErrors = false,
				.WinHttpSession = 
					Boring32::WinHttp::Session(L"testUserAgent")
					// Uncomment to use named proxy
					//Boring32::WinHttp::Session(L"testUserAgent", L"125.164.86.89:3128")
			}
		);
		socket.Connect();
		std::vector<char> buffer;
		socket.SendString("Hello!");
		socket.Receive(buffer);
		std::string response(buffer.begin(), buffer.end());
		std::wcout << response.c_str() << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::wcout << ex.what() << std::endl;
	}

	std::wcout << Boring32::Util::GetCurrentExecutableDirectory() << std::endl;

	//TestProcessNamedPipe();
	for (int i = 0; i < 14; i++)
	{
		try
		{
			std::wcout << L"Test: " << i << std::endl;
			if (i == 0)
				TestProcessOverlappedNamedPipe();
			if (i == 1)
				TestThreadSafeVector();
			if (i == 2)
				TestException();
			if (i == 3)
				TestWaitableTime(2000);
			if (i == 4)
				TestSemaphore();
			if (i == 5)
				TestMutex();
			if (i == 6)
				TestConversions();
			if (i == 7)
				TestMemoryMappedFile();
			if (i == 8)
				TestAnonPipes();
			if (i == 9)
				TestLibraryLoad();
			if (i == 10)
				TestProcessBlockingNamedPipe();
			if (i == 11)
				TestProcessAnonPipe();
			if (i == 12)
				TestCompression();
			if (i == 13)
				TestTimerQueues();
		}
		catch (const std::exception& ex)
		{
			std::wcout << ex.what() << std::endl;
		}
	}
	
	//Boring32::WinHttp::HttpWebClient client(
	//	L"TestClientAgent", 
	//	L"127.0.0.1", 
	//	94873, 
	//	true, 
	//	{ L"application/json" },
	//	L"Content-Type: application/json"
	//);
	//client.Post(L"/some/object", "");

	return 0;
}

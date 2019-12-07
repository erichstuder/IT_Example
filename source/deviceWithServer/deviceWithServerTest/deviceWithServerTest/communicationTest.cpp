/* IT - Internal Tracer
 * Copyright (C) 2019 Erich Studer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <://www.gnu.org/licenses/>.
 */

#include "pch.h"
#include <Python.h>

das muss man wohl so machen:
https://stackoverflow.com/questions/1147452/create-instance-of-a-python-class-declared-in-python-with-c-api


class CommunicationTest : public ::testing::Test {
protected:
	CommunicationTest() {}

	virtual ~CommunicationTest() {}

	virtual void SetUp() {
		//see: https://docs.python.org/3/extending/embedding.html
		const wchar_t ProgramName[] = L"communicationTest.cpp";
		Py_SetProgramName(ProgramName);
		Py_Initialize();

		PyObject* pythonModuleName;
		PyObject* pythonModule;
		PyObject* clientTelegramParserConstructor;
		PyObject* pythonTelegramParserInstance;
		PyObject* clientTelegramParserObject;
		PyObject* clientParseFunction;
		pythonModuleName = PyUnicode_DecodeFSDefault("TelegramParser");
		pythonModule = PyImport_Import(pythonModuleName);
		Py_DECREF(pythonModuleName);
		if (pythonModule != NULL) {
			clientTelegramParserConstructor = PyObject_GetAttrString(pythonModule, "TelegramParser"); //TODO: remeove string
			
			pythonTelegramParserInstance = PyInstance_New(clientTelegramParserConstructor, NULL, NULL);

			
			clientTelegramParserConstructor = PyObject_GetAttrString(pythonModule, "TelegramParser");
			
			if (clientTelegramParserConstructor && PyCallable_Check(clientTelegramParserConstructor)) {
				
				
				
				
				clientTelegramParserObject = PyObject_CallObject(clientTelegramParserConstructor, NULL);
				
				
				//pArgs = PyTuple_New(argc - 3);
				//for (i = 0; i < argc - 3; ++i) {
				//	pValue = PyLong_FromLong(atoi(argv[i + 3]));
				//	if (!pValue) {
				//		Py_DECREF(pArgs);
				//		Py_DECREF(pModule);
				//		fprintf(stderr, "Cannot convert argument\n");
				//		return 1;
				//	}
				//	/* pValue reference stolen here: */
				//	PyTuple_SetItem(pArgs, i, pValue);
				//}
				//pValue = PyObject_CallObject(pFunc, pArgs);
				//Py_DECREF(pArgs);
				//if (pValue != NULL) {
				//	printf("Result of call: %ld\n", PyLong_AsLong(pValue));
				//	Py_DECREF(pValue);
				//}
				//else {
				//	Py_DECREF(pFunc);
				//	Py_DECREF(pModule);
				//	PyErr_Print();
				//	fprintf(stderr, "Call failed\n");
				//	return 1;
				}
		}

		PyRun_SimpleString("from time import time,ctime\n"
			"print ('Today is:' ,ctime(time()))\n");
	}

	virtual void TearDown() {
		Py_Finalize();
	}
};


TEST_F(CommunicationTest, tickFunctionExists) {
	//PyObject_GetAttrString(pModule, argv[2]);
}

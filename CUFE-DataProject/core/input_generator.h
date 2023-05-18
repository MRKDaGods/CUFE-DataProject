#pragma once

#include "../common.h"

#include <string>

namespace core {
	/// <summary>
	/// An input file raw model
	/// </summary>
	struct InputFileModel {
		_STD wstring filename = L"Input file test.txt";

		_STD wstring proc_count = L"500";

		_STD wstring fcfs_count = L"5";
		_STD wstring sjf_count = L"4";
		_STD wstring rr_count = L"2";
		_STD wstring edf_count = L"2";

		_STD wstring rr_slice = L"3";
		_STD wstring rtf = L"4";
		_STD wstring maxw = L"20";
		_STD wstring stl = L"100";
		_STD wstring fork = L"10";
		_STD wstring overheat = L"10";

		bool generate_sigkills = true;
		bool enable_edf = true;
	};

	void GenerateInputFile(InputFileModel* model);
}
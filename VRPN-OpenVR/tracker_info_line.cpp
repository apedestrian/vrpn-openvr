#include "tracker_info_line.h"
#include <iostream>

HANDLE tracker_info_line::console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

tracker_info_line::tracker_info_line(const std::string& name)
	: tracker_name(name), last_char_count(0), last_result(vr::ETrackingResult::TrackingResult_Running_OK)
{
	// Set cursor position for this tracked device
	line_coord.X = 0;
	line_coord.Y = get_console_cursor_position().Y;

	// Construct initial connection output
	std::string output = "[" + tracker_name + "]: Tracking";
	last_char_count = output.length();
	std::cout << output << std::endl;
}

void tracker_info_line::update_status_and_print(vr::ETrackingResult new_result)
{
	if (new_result != last_result) {

		last_result = new_result;

		// Snapshot current cursor position
		COORD cursor_position = get_console_cursor_position();

		// Start string buffer
		std::string output = "[" + tracker_name + "] ";

		switch (new_result)
		{
		case vr::TrackingResult_Uninitialized:
			output += "Uninitialized";
			break;
		case vr::TrackingResult_Calibrating_InProgress:
			output += "Calibrating (In Progress)";
			break;
		case vr::TrackingResult_Calibrating_OutOfRange:
			output += "Calibrating (Out of Range)";
			break;
		case vr::TrackingResult_Running_OK:
			output += "Tracking";
			break;
		case vr::TrackingResult_Running_OutOfRange:
			output += "Running (Out of Range)";
			break;
		default:
			output += "Unknown tracking result";
			break;
		}

		// Write over trailing characters from last output and snapshot new length
		size_t new_char_count = output.length();
		while (output.length() < last_char_count) {
			output += " ";
		}
		last_char_count = new_char_count;

		// Finally, send output to console
		SetConsoleCursorPosition(console_handle, line_coord);
		std::cout << output;


		// Return cursor to its position
		SetConsoleCursorPosition(console_handle, cursor_position);
	}
}

COORD tracker_info_line::get_console_cursor_position()
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(console_handle, &cbsi))
	{
		return cbsi.dwCursorPosition;
	}
	else
	{
		// The function failed. Call GetLastError() for details.
		COORD invalid = { 0, 0 };
		return invalid;
	}
}

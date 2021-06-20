/*
MIT License

Copyright (c) 2021 Mehmet Ali Baykara

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Process.h"

Process::Process() {};
Process::~Process() {};

Process::Process(const int& _pid, const int& _ppid, const std::string& _command, const std::string& _directory)
	: pid(_pid), ppid(_ppid), command(_command), directory(_directory), children(0){}

/*
* Add child to the tree
*/
void Process::InsertChild(Process& proc)
{
	this->children.push_back(proc);
}
/*
* Traverse and find parent
* insert new process as a child
*/
void Process::TraverseAndInsertChild(Process& p) 
{
	for (auto& c : this->children)
	{
		if (p.ppid == c.pid)
		{
				c.InsertChild(p);
				break;
		}else c.TraverseAndInsertChild(p);
	}
	
}

/*
* It accesses each Process objects parameters @command and @directory
* and replaces backslash and recursively traverse Tree structure
* and streams as json format
*/
std::string Process::GetJSON()
{
	std::replace(this->command.begin(), this->command.end(), '\"', '\'');
	std::replace(this->directory.begin(), this->directory.end(), '\"', '\'');
	std::stringstream ret_json;
	ret_json << "{" << "\"directory\""<<":\"" << this->directory<< "\","
		<< "\"Command\":" << "\""<< this->command << "\"," 	<< "\"Children\":[";
	bool first = true;
	for (auto child : this->children) 
	{
		if (first) { first = false;	}
		else { ret_json << ",";}
		ret_json << child.GetJSON();
	}
	ret_json << "]}";
	return ret_json.str();
}


// source file for `Student_info'-related functions
#include "Student_info.h"

using std::istream;  using std::vector;

bool compare(const Student_info& x, const Student_info& y)
{
	return x.name < y.name;
}

istream& read(istream& is, Student_info& s)
{
	// read and store the student's name and midterm and final exam grades
	is >> s.name >> s.midterm >> s.final;

	read_hw(is, s.homework);  // read and store all the student's homework grades
	return is;
}

// read homework grades from an input stream into a `vector<double>'
istream& read_hw(istream& in, double& hw)
{
	if (in) {
		// read homework grades
		double x;
		double sum = 0.0;
		int count = 0;
		while (in >> x){
			sum += x;
			count++;
		}
		if(count == 0){
			hw = 0.0;
		}else{
			hw = sum/count;
		}

		// clear the stream so that input will work for the next student
		in.clear();

	}
	return in;
}

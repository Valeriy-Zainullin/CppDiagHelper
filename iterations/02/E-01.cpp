#include <map>

class Student {
public:
	Student(int markParam)
		: mark(markParam) {}
private:
	int mark;
};

int main() {
	std::map<int, Student> map;
	map[1] = Student(1);
	return 0;
}

#include <iostream>
#include <cmath>

void test(float delta, float omega) {
	const float S_0 = 10;
	for(auto t = 0; t < 10; t++) {
		float s_t = (S_0 * exp(-delta * t) * sin(omega * t));
		std::cout << s_t << '\n';
	}
}

int main(int argc, char **argv) {
	for(auto i = 0; i < 10; i++) {
		std::cout << "omega: " << i << '\n';
		test(0.1, i);

		std::cin.get();
	}
}


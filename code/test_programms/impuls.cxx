#include <iostream>
#include <cmath>

void test(float delta, float omega) {
	const float S_0 = 0.1;
	for(auto t = 0; t < 10; t++) {
		float s_t = S_0 * exp(-delta * t) *
			((1 + omega * S_0) / omega * sin(omega * t) +
			 S_0 + cos(omega * t));
		std::cout << s_t << '\n';
	}
}

int main(int argc, char **argv) {
	test(1, 3);

	for(auto i = 0; i < 10; i++) {
		std::cout << "delta: " << i << '\n';
		test(i, 1);

		std::cin.get();
	}
}


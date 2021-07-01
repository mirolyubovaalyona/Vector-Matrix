#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Matrix.h"
#include "Vector.h"
#include "Base.h"


namespace mat_vec {

	TEST_CASE("Vector") {
		SECTION("Vector") {
			Vector c(8, 0);
			Vector vec3(8, -9);
			REQUIRE(c[1] == 0);
			REQUIRE(c.size() == 8);
			Vector vec1(15, -7);
			REQUIRE(vec1[7] == -7);
			REQUIRE(vec1.size() == 15);
			Vector vec2 = c;
			for (size_t i = 0; i < c.size(); i++) REQUIRE(c[i] == 0);
			REQUIRE(vec2.size() == 8);

			// -vector + vector
			Vector vec4 = c + vec3;
			c += vec3;
			for (size_t i = 0; i < vec4.size(); ++i) {
				REQUIRE(vec4[i] == -9);
				REQUIRE(vec4[i] == c[i]);
			}

			// -vector - vector
			Vector vec5 = c - vec3;
			c -= vec3;
			for (size_t i = 0; i < vec5.size(); ++i) {
				REQUIRE(vec5[i] == 0);
				REQUIRE(vec5[i] == c[i]);
			}

			Vector vec6 = vec4 ^ vec3;
			vec4 ^= vec3;
			for (size_t i = 0; i < vec6.size(); ++i) {
				REQUIRE(vec6[i] == 81);
				REQUIRE(vec6[i] == vec4[i]);
			}

			// - vector * vector
			REQUIRE(vec6 * vec3 == -5832);

			// - vector / const
			Vector vec7 = vec6 / 3;
			vec6 /= 3;
			for (size_t i = 0; i < vec6.size(); ++i) {
				REQUIRE(vec7[i] == 27);
				REQUIRE(vec6[i] == vec7[i]);
			}

			// - vector * const
			vec7 = vec6 * 3;
			vec6 *= 3;
			for (size_t i = 0; i < vec6.size(); ++i) {
				REQUIRE(vec7[i] == 81);
				REQUIRE(vec6[i] == vec7[i]);
			}

			//-vector * matrix

			Matrix mat_test(3, 2.0);
			Vector v1(3, 1);
			Vector v2 = v1 * mat_test;
			v1 *= mat_test;
			for (size_t i = 0; i < 3; ++i) {
				REQUIRE(v2[i] == 6);
				REQUIRE(v1[i] == 6);
			}

			//-equality vector == vector
			Vector v6(3, 777);
			REQUIRE(v2 == v1);
			v1[1] = -55555.0;
			REQUIRE(v2 != v6);
			REQUIRE(v6 != v1);

		}
		SECTION("Functions") {
			Vector test_vec(3, 2.0);
			Vector test_vec2(4, 5.0);
			// -function norm
			test_vec[0] = 1.0;
			REQUIRE(test_vec.norm() == 3);
			REQUIRE(test_vec2.norm() == 10);

			// function normalized

			Vector test_vec3 = test_vec2.normalized();
			REQUIRE(test_vec3[0] == test_vec2[0] / 10);
			test_vec3 = test_vec.normalized();
			REQUIRE(test_vec3[0] == test_vec[0] / 3.0);

			// function normalize

			test_vec.normalize();
			REQUIRE(test_vec[0] == test_vec3[0]);
			REQUIRE(test_vec[1] == test_vec3[1]);
			REQUIRE(test_vec[2] == test_vec3[2]);
		}





		SECTION("Matrix") {
			SECTION("Constructors") {
				Matrix mat_test(2, 3.0);
				for (size_t i = 0; i < mat_test.shape().first; ++i) {
					for (size_t j = 0; j < mat_test.shape().second; ++j) {
						REQUIRE(mat_test.get(i, j) == 3.0);
					}
				}

				Matrix mat_test2(2, 2, 5.0);
				for (size_t i = 0; i < mat_test2.shape().first; ++i) {
					for (size_t j = 0; j < mat_test2.shape().second; ++j) {
						REQUIRE(mat_test2.get(i, j) == 5.0);
					}
				}

				Matrix mat_test3 = mat_test;
				for (size_t i = 0; i < mat_test.shape().first; ++i) {
					for (size_t j = 0; j < mat_test.shape().second; ++j) {
						REQUIRE(mat_test.get(i, j) == mat_test3.get(i, j));
					}
				}
			}
			SECTION("11") {

				Matrix mat_test(2, 3.0);
				Matrix mat_test2(2,4.0);
				Matrix mat_test1 = mat_test;
				REQUIRE(mat_test == mat_test);
				REQUIRE(mat_test != mat_test2);
				Matrix mat_test4 = mat_test + mat_test2;
				mat_test += mat_test2;
				REQUIRE(mat_test4 == mat_test);
				Matrix mat_test3 = mat_test - mat_test2;
				mat_test -= mat_test2;
				REQUIRE(mat_test3 == mat_test);
				Matrix mat_test5(2, 1.0);
				Matrix mat_test6= mat_test5*5;
				mat_test5 *= 5;
				REQUIRE(mat_test5 == mat_test6);
				Matrix mat_test7 = mat_test5 / 5;
				mat_test5 /= 5;
				REQUIRE(mat_test5 == mat_test7);
				Matrix mat_test8= mat_test5* mat_test6;
				mat_test5*= mat_test6;
				REQUIRE(mat_test8 == mat_test5);
				Matrix mat_test9(9, 4, 7.0);
				Matrix mat_test10 = mat_test9.transposed();
				mat_test9.transpose();
				REQUIRE(mat_test9 == mat_test10);
				Matrix mat_test11(11, 2, 2.0);
				Matrix mat_test12(2, 11, 2.0);
				mat_test11.reshape(11, 2);
				REQUIRE(mat_test11 == mat_test12);
				Matrix mat_test13(4, 5.0);
				Vector vec13(4, 1);
				Vector v4 = mat_test13 * vec13;
				Vector v5(4, 20.0);
				REQUIRE(v5 == v4);

			
			}

			
		}







	}
}

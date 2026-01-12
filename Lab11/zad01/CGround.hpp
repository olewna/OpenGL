#ifndef __CGROUND_HPP
#define __CGROUND_HPP
#define __ALTITUDE_ERROR 99999

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CProgram.hpp"
#include "CMesh.hpp"

class CTriangle
{
public:
	// Konstruktory
	CTriangle() = default;
	CTriangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
		: p{p1, p2, p3}
	{
		calculateEquation();
	}

	[[nodiscard]] inline bool isAbove(const glm::vec2 &point) const
	{
		float d1 = det(point, p[0], p[1]);
		float d2 = det(point, p[1], p[2]);
		float d3 = det(point, p[2], p[0]);

		bool isNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
		bool isPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

		if (isNeg && isPos)
			return false;
		else
			return true;
	}

	[[nodiscard]] inline float calcY(const glm::vec2 &point) const
	{
		if (B)
			return -(A * point.x + C * point.y + D) / B;
		else
		{
			return __ALTITUDE_ERROR;
		}
	}

private:
	void calculateEquation(void)
	{
		glm::vec3 v1 = p[1] - p[0];
		glm::vec3 v2 = p[2] - p[0];

		glm::vec3 normal = glm::cross(v1, v2);

		A = normal.x;
		B = normal.y;
		C = normal.z;

		D = -(A * p[0].x + B * p[0].y + C * p[0].z);

		if (B == 0)
		{
			std::cerr << "Uwaga! Trojkat pionowy.\n";
		}
	}

	[[nodiscard]] static float det(glm::vec2 p, glm::vec3 A, glm::vec3 B)
	{
		return (p.x - B.x) * (A.z - B.z) - (A.x - B.x) * (p.y - B.z);
	}

	glm::vec3 p[3];
	float A, B, C, D;
};

// ---------------------------------------
// Klasa reprezentujaca podloze
// ---------------------------------------
class CGround
{
public:
	CGround() = default;

	// =============================
	// Inicjalizacja
	// =============================
	bool Init(const char *objFile, const char *texFile)
	{
		if (!mesh.CreateFromOBJ(objFile))
		{
			std::cerr << "Ground OBJ load failed\n";
			return false;
		}

		mesh.LoadTexture(texFile);

		BuildCollisionMesh();

		return true;
	}

	void Draw(CProgram &program)
	{
		program.SetMat4("matModel", mesh.GetModelMatrix());
		mesh.Draw(program);
	}

	float getY(const glm::vec2 &point) const
	{
		for (const auto &tri : triangles)
		{
			if (tri.isAbove(point))
			{
				return tri.calcY(point);
			}
		}

		return std::numeric_limits<float>::quiet_NaN();
	}

private:
	void BuildCollisionMesh()
	{
		const auto &verts = mesh.GetVertices();

		triangles.clear();
		triangles.reserve(verts.size() / 3);

		for (size_t i = 0; i < verts.size(); i += 3)
		{
			triangles.emplace_back(
				verts[i],
				verts[i + 1],
				verts[i + 2]);
		}

		std::cout << "Ground collision triangles: "
				  << triangles.size() << "\n";
	}

private:
	CMesh mesh;
	std::vector<CTriangle> triangles;
};

#endif
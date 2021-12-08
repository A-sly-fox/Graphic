#include <vector>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>
#include <algorithm>
using namespace std;
#define MAX_DEPTH 5

int mymin(float a, float b, float c) {
	int i = 0;
	float min = a;
	if (min > b) {
		min = b;
		i = 1;
	}
	if (min > c) {
		min = c;
		i = 2;
	}
	return i;
}

int mymax(float a, float b, float c) {
	int i = 0;
	float max = a;
	if (max < b) {
		max = b;
		i = 1;
	}
	if (max < c) {
		max = c;
		i = 2;
	}
	return i;
}

//define class Vec_3, used in ray direction
//����Vec_3�࣬���ڹ���׷��
template<typename T>
class Vec_3
{
public:
	//����x,y,z��������
	T x, y, z;
	//���캯��
	Vec_3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec_3(T xx) : x(xx), y(xx), z(xx) {}
	Vec_3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	//���� * ����
	Vec_3<T> operator * (const T& f) const {
		return Vec_3<T>(x * f, y * f, z * f);
	}
	Vec_3<T> operator * (const Vec_3<T>& v) const {
		return Vec_3<T>(x * v.x, y * v.y, z * v.z);
	}
	//�����������
	T dot(const Vec_3<T>& v) const {
		return x * v.x + y * v.y + z * v.z;
	}
	//���� - ����
	Vec_3<T> operator - (const Vec_3<T>& v) const {
		return Vec_3<T>(x - v.x, y - v.y, z - v.z);
	}
	//���� + ����
	Vec_3<T> operator + (const Vec_3<T>& v) const {
		return Vec_3<T>(x + v.x, y + v.y, z + v.z);
	}
	Vec_3<T>& operator += (const Vec_3<T>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vec_3<T>& operator *= (const Vec_3<T>& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vec_3<T> operator - () const {
		return Vec_3<T>(-x, -y, -z);
	}
	//���������ĳ���
	T length2() const {
		return x * x + y * y + z * z;
	}
	T length() const {
		return sqrt(length2());
	}
	//���������й�һ��
	Vec_3& normal() {
		T nor2 = length2();
		if (nor2 > 0)
		{
			T nor2_inv = 1 / sqrt(nor2);
			x *= nor2_inv;
			y *= nor2_inv;
			z *= nor2_inv;
		}
		return *this;
	}
	//�����������Ϣ
	friend std::ostream& operator << (std::ostream& os, const Vec_3<T>& v) {
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};

typedef Vec_3<float> Vec_3f;

//����������
class Sphere
{
public:
	Vec_3f center; //����
	float radius, radius2; //��뾶����뾶ƽ��
	Vec_3f surfaceColor, emissionColor; //�������ɫ�����Է�����ɫ(��Դ)
	float transparency, reflection; //���͸��ȡ������
	//��Ĺ��캯��
	Sphere(
		const Vec_3f& c,
		const float& r,
		const Vec_3f& sc,
		const float& refl = 0,
		const float& transp = 0,
		const Vec_3f& ec = 0) :
		center(c), radius(r), radius2(r* r), surfaceColor(sc),
		emissionColor(ec), transparency(transp), reflection(refl)
	{}
	//ʹ�ü��η�ʽ, �����������Ľ���, ���������t0, t1
	bool intersect(const Vec_3f& rayorigin, const Vec_3f& raydirection, float& t0, float& t1) const
	{
		//���������㵽�������ĵ�����
		Vec_3f l = center - rayorigin;
		//���ߵķ��������λ���෴,���ཻ
		float tca = l.dot(raydirection);
		if (tca < 0) return false;
		//������������ĵ���̾���
		float dist = l.dot(l) - tca * tca;
		//������ڰ뾶,���ཻ
		if (dist > radius2) return false;
		float thc = sqrt(radius2 - dist);
		//t0:��һ���ཻ����; t1:�ڶ����ཻ����
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}
};

Vec_3f hitplane_normal;
//����������
class Cube
{
public:
	Vec_3f center; //����
	float half_length; //���ⳤ
	Vec_3f cube_normal;
	Vec_3f surfaceColor, emissionColor; //�����������ɫ���������Է�����ɫ(��Դ)
	float transparency, reflection; //�������͸��ȡ������
	//������Ĺ��캯��
	Cube(
		const Vec_3f& c,
		const float& hl,
		const Vec_3f& sc,
		const Vec_3f& cn,
		const float& refl = 0,
		const float& transp = 0,
		const Vec_3f& ec = 0) :
		center(c), half_length(hl), surfaceColor(sc), cube_normal(cn),
		emissionColor(ec), transparency(transp), reflection(refl)
	{}
	//ʹ�ü��η�ʽ, ���������������Ľ���, ���������t0
	bool intersect(const Vec_3f& rayorigin, const Vec_3f& raydirection, float& t0) const
	{
		Vec_3f normal[6];
		normal[0] = cube_normal;
		normal[1] = -cube_normal;
		normal[2].x = cube_normal.x;
		normal[2].y = cube_normal.z;
		normal[2].z = -cube_normal.y;
		normal[3] = -normal[2];
		normal[4].x = -cube_normal.z;
		normal[4].y = cube_normal.y;
		normal[4].z = cube_normal.x;
		normal[5] = -normal[4];
		float tmin[3];
		float tmax[3];
		Vec_3f facemin[3];
		Vec_3f u_normal = 0;
		hitplane_normal = 0;
		for (int i = 0; i < 3; i++)
		{
			float q = raydirection.dot(normal[i * 2]);
			if (q == 0) {
				tmin[i] = INFINITY;
				tmax[i] = -INFINITY;
			}
			else {
				Vec_3f vertex_p0 = center + normal[i * 2] * half_length;
				float D0 = -vertex_p0.dot(normal[i * 2]);
				float A0 = D0 + rayorigin.dot(normal[i * 2]);
				float B0 = raydirection.dot(normal[i * 2]);
				float result0 = -A0 / B0;
				Vec_3f vertex_p1 = center + normal[i * 2 + 1] * half_length;
				float D1 = -vertex_p1.dot(normal[i * 2 + 1]);
				float A1 = D1 + rayorigin.dot(normal[i * 2 + 1]);
				float B1 = raydirection.dot(normal[i * 2 + 1]);
				float result1 = -A1 / B1;
				if (result0 < result1) {
					tmin[i] = result0;
					tmax[i] = result1;
					facemin[i] = normal[i * 2];
				}
				else {
					tmin[i] = result1;
					tmax[i] = result0;
					facemin[i] = normal[i * 2 + 1];
				}
			}
		}
		int tminmax = mymax(tmin[0], tmin[1], tmin[2]);
		int tmaxmin = mymin(tmax[0], tmax[1], tmax[2]);
		if (tmin[tminmax] < tmax[tmaxmin]) {
			t0 = tmin[tminmax];
			hitplane_normal = facemin[tminmax];
			return true;
		}
		else {
			return false;
		}
	}
};

//����׷�ٺ���,������ߵ���㡢���򡢳����е����塢����׷�ٵ�������
Vec_3f trace(
	const Vec_3f& rayorigin,
	const Vec_3f& raydirection,
	const std::vector<Sphere>& spheres,
	const std::vector<Cube>& cubes,
	const int& depth
)
{
	int issphere = 1; //��ʾ����ཻ�������ǲ�������
	float tnear = INFINITY; //������ཻ����
	const Sphere* sphere = NULL; //������ཻ����
	const Cube* cube = NULL; //������ཻ����
	Vec_3f new_hitpoint_normal = 0; //���㽻��ķ�����

	//�볡���е�������,�ҵ�����Ľ���
	for (unsigned i = 0; i < spheres.size(); i++)
	{
		float t0 = INFINITY;
		float t1 = INFINITY;
		if (spheres[i].intersect(rayorigin, raydirection, t0, t1))
		{
			if (t0 < 0) t0 = t1;
			if (t0 < tnear)
			{
				tnear = t0;
				sphere = &spheres[i];
			}
		}
	}
	for (unsigned i = 0; i < cubes.size(); i++)
	{
		float t0 = INFINITY;
		Vec_3f hitpoint_normal = 0;
		if (cubes[i].intersect(rayorigin, raydirection, t0))
		{
			if (t0 < tnear)
			{
				new_hitpoint_normal = hitpoint_normal;
				tnear = t0;
				cube = &cubes[i];
				issphere = 0;
			}
		}
	}
	if (issphere) {
		//���߲��볡���е������ཻ,���ر�����ɫ,��ɫ
		if (!sphere) return Vec_3f(2);

		Vec_3f surfaceColor = 0; //����������ɫ
		Vec_3f phit = rayorigin + raydirection * tnear; //���㽻��
		Vec_3f nhit = phit - sphere->center; //���㽻��ָ�����ĵ�����nhit,Ϊ���淨��
		nhit.normal(); //����
		//nhit����ߵĵ��Ϊ��,˵���������λ��������
		float bias = 1e-4;
		bool inside = false;
		if (raydirection.dot(nhit) > 0)
		{
			nhit = -nhit;
			inside = true;
		}
		//������ߵķ���������, ���������ɫ
		if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_DEPTH)
		{
			float facingratio = -raydirection.dot(nhit);
			float fresneleffect = pow(1 - facingratio, 3) * 0.9 + 1 * 0.1;
			//���㷴�䷽��
			Vec_3f reflect_direction = raydirection - nhit * 2 * raydirection.dot(nhit);
			reflect_direction.normal();
			//�ݹ���ù��߸��ٺ���
			Vec_3f next_reflection = trace(phit + nhit * bias, reflect_direction, spheres, cubes, depth + 1);
			Vec_3f next_refraction = 0;
			//ֻ�е�������͸��Ч�����ż���͸��
			if (sphere->transparency)
			{
				//�ж�͸����ڲ໹�����
				float ior = 1.1, eta = (inside) ? ior : 1 / ior;
				//����͸��ĽǶ�
				float cosi = -nhit.dot(raydirection);
				float k = 1 - eta * eta * (1 - cosi * cosi);
				Vec_3f refraction_direction = raydirection * eta + nhit * (eta * cosi - sqrt(k));
				refraction_direction.normal();
				//�ݹ���ù��߸��ٺ���
				next_refraction = trace(phit - nhit * bias, refraction_direction, spheres, cubes, depth + 1);
			}
			//�������ɫ�Ƿ����������ɫ�ĵ���, surfaceColor������ɫ�����ı���
			surfaceColor = (next_reflection * fresneleffect + next_refraction * (1 - fresneleffect) * sphere->transparency) * sphere->surfaceColor;
		}
		//���淴��Ⱥ�Ͷ���Ϊ0,����Ҫ����׷��;�ﵽ���ݹ����,��ֹ���߸���
		else {
			for (unsigned i = 0; i < spheres.size(); i++)
			{
				//�ҵ��䵱��Դ������
				if (sphere[i].emissionColor.x > 0)
				{
					Vec_3f transmission = 1;
					//������߷���
					Vec_3f lightDirection = sphere[i].center - phit;
					lightDirection.normal();
					//�����Դ�뽻����Ƿ�����������,�����ӰЧ��
					for (unsigned j = 0; j < spheres.size(); ++j) {
						if (i != j) {
							float t0, t1;
							if (spheres[j].intersect(phit + nhit * bias, lightDirection, t0, t1))
							{
								transmission = 0;
								break;
							}
						}
					}
					//��������ı�����ɫ
					surfaceColor += sphere->surfaceColor * transmission * std::max(float(0), nhit.dot(lightDirection)) * spheres[i].emissionColor;
				}
			}
		}
		// �������յı�����ɫ
		return surfaceColor + sphere->emissionColor;
	}
	else {
		Vec_3f surfaceColor = 0; //����������ɫ
		Vec_3f phit = rayorigin + raydirection * tnear; //���㽻��
		Vec_3f nhit = new_hitpoint_normal; //���淨��
		nhit.normal(); //����
		//nhit����ߵĵ��Ϊ��,˵���������λ������
		float bias = 1e-4;
		bool inside = false;
		if (raydirection.dot(nhit) > 0)
		{
			nhit = -nhit;
			inside = true;
		}
		//������ߵķ���������, ���������ɫ
		if ((cube->transparency > 0 || cube->reflection > 0) && depth < MAX_DEPTH)
		{
			float facingratio = -raydirection.dot(nhit);
			float fresneleffect = pow(1 - facingratio, 3) * 0.9 + 1 * 0.1;
			//���㷴�䷽��
			Vec_3f reflect_direction = raydirection - nhit * 2 * raydirection.dot(nhit);
			reflect_direction.normal();
			//�ݹ���ù��߸��ٺ���
			Vec_3f next_reflection = trace(phit + nhit * bias, reflect_direction, spheres, cubes, depth + 1);
			Vec_3f next_refraction = 0;
			//ֻ�е�������͸��Ч�����ż���͸��
			if (cube->transparency)
			{
				//�ж�͸����ڲ໹�����
				float ior = 1.1, eta = (inside) ? ior : 1 / ior;
				//����͸��ĽǶ�
				float cosi = -nhit.dot(raydirection);
				float k = 1 - eta * eta * (1 - cosi * cosi);
				Vec_3f refraction_direction = raydirection * eta + nhit * (eta * cosi - sqrt(k));
				refraction_direction.normal();
				//�ݹ���ù��߸��ٺ���
				next_refraction = trace(phit - nhit * bias, refraction_direction, spheres, cubes, depth + 1);
			}
			//�������ɫ�Ƿ����������ɫ�ĵ���, surfaceColor������ɫ�����ı���
			surfaceColor = (next_reflection * fresneleffect + next_refraction * (1 - fresneleffect) * cube->transparency) * cube->surfaceColor;
		}
		//���淴��Ⱥ�Ͷ���Ϊ0,����Ҫ����׷��;�ﵽ���ݹ����,��ֹ���߸���
		else {
			for (unsigned i = 0; i < spheres.size(); i++)
			{
				//�ҵ��䵱��Դ������
				if (spheres[i].emissionColor.x > 0)
				{
					Vec_3f transmission = 1;
					//������߷���
					Vec_3f lightDirection = spheres[i].center - phit;
					lightDirection.normal();
					//�����Դ�뽻����Ƿ�����������,�����ӰЧ��
					for (unsigned j = 0; j < spheres.size(); ++j) {
						if (i != j) {
							float t0, t1;
							if (spheres[j].intersect(phit + nhit * bias, lightDirection, t0, t1))
							{
								transmission = 0;
								break;
							}
						}
					}
					//��������ı�����ɫ
					surfaceColor += cube->surfaceColor * transmission * std::max(float(0), nhit.dot(lightDirection)) * spheres[i].emissionColor;
				}
			}
		}
		// �������յı�����ɫ
		return surfaceColor + cube->emissionColor;
	}
}

//��Ⱦ����,����ÿһ�����ص���ɫ
void render(const std::vector<Sphere>& spheres, const std::vector<Cube>& cubes)
{
	int width, height;
	float fov;
	cout << "input: width height fov" << endl;
	scanf("%d %d %f", &width, &height, &fov);
	Vec_3f* img = new Vec_3f[width * height], * pixel = img;
	float invwidth = 1 / float(width), invHeight = 1 / float(height);
	float aspectratio = width / float(height);
	float angle = tan(M_PI * 0.5 * fov / 180.);
	//�����е����ؽ��й���׷��
	for (unsigned y = 0; y < height; y++)
	{
		for (unsigned x = 0; x < width; x++, pixel++)
		{
			float xx = (2 * ((x + 0.5) * invwidth) - 1) * angle * aspectratio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Vec_3f raydir(xx, yy, -1);
			raydir.normal();
			*pixel = trace(Vec_3f(0), raydir, spheres, cubes, 0);
		}
	}
	//������Ⱦ�����ս��
	std::ofstream ofs("./trace.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; i++)
	{
		//0,255
		ofs << (unsigned char)(std::min(float(1), img[i].x) * 255)
			<< (unsigned char)(std::min(float(1), img[i].y) * 255)
			<< (unsigned char)(std::min(float(1), img[i].z) * 255);
	}
	ofs.close();
	delete[] img;
}

// ��������, ����5�����塢1����Դ(��ԴҲ������), ��Ⱦ���յĽ��
int main(int argc, char* argv[])
{
	std::vector<Sphere> spheres;
	std::vector<Cube> cubes;
	// argument: position, radius, surfacecolor, reflectivity, transparency, emissionColor 
	// λ��, �뾶, ������ɫ, ������, ͸����, �Է���

	// Cube: ���ģ����ⳤ��������ɫ��һ����ķ������������ʣ������ʣ��Է���
	spheres.push_back(Sphere(Vec_3f(0.0, -10004, -20), 10000, Vec_3f(0.20, 0.20, 0.20), 0, 0.0));
	spheres.push_back(Sphere(Vec_3f(0.0, 0, -20), 4, Vec_3f(1.00, 0.32, 0.36), 1, 0.5));
	spheres.push_back(Sphere(Vec_3f(5.0, -1, -15), 2, Vec_3f(0.90, 0.76, 0.46), 1, 0.0));
	spheres.push_back(Sphere(Vec_3f(5.0, 0, -25), 3, Vec_3f(0.65, 0.77, 0.97), 1, 0.0));
	spheres.push_back(Sphere(Vec_3f(-5.5, 0, -15), 3, Vec_3f(0.90, 0.90, 0.90), 1, 0.0));
	cubes.push_back(Cube(Vec_3f(10, 0, -20), 2, Vec_3f(0.0, 0.0, 0.1), Vec_3f(0.0, 0.0, 1.0), 1, 0.0));
	//Light
	//��Դ
	spheres.push_back(Sphere(Vec_3f(0.0, 20, -30), 3, Vec_3f(0.0, 0.0, 0.0), 0, 0.0, Vec_3f(1)));
	render(spheres, cubes);

	return 0;
}
/*
//������Ĺ��캯��
Cube(
	const Vec_3f& c,
	const float& hl,
	const Vec_3f& sc,
	const Vec_3f& cn,
	const float& refl = 0,
	const float& transp = 0,
	const Vec_3f& ec = 0) :
	center(c), half_length(hl), surfaceColor(sc), cube_normal(cn),
	emissionColor(ec), transparency(transp), reflection(refl)
{}
*/
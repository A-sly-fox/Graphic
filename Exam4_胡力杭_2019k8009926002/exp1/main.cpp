#include <vector>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>
#include <algorithm>
using namespace std;
#define MAX_DEPTH 20

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

//����׷�ٺ���,������ߵ���㡢���򡢳����е����塢����׷�ٵ�������
Vec_3f trace(
	const Vec_3f& rayorigin,
	const Vec_3f& raydirection,
	const std::vector<Sphere>& spheres,
	const int& depth
)
{
	float tnear = INFINITY; //������ཻ����
	const Sphere* sphere = NULL; //������ཻ����
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
		Vec_3f next_reflection = trace(phit + nhit * bias, reflect_direction, spheres, depth + 1);
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
			next_refraction = trace(phit - nhit * bias, refraction_direction, spheres, depth + 1);
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

//��Ⱦ����,����ÿһ�����ص���ɫ
void render(const std::vector<Sphere>& spheres)
{
	unsigned width = 640, height = 480;
	Vec_3f* img = new Vec_3f[width * height], * pixel = img;
	float invwidth = 1 / float(width), invHeight = 1 / float(height);
	float fov = 30;
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
			*pixel = trace(Vec_3f(0), raydir, spheres, 0);
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
int main()
{
	std::vector<Sphere> spheres;
	// argument: position, radius, surfacecolor, reflectivity, transparency, emissionColor 
	// λ��, �뾶, ������ɫ, ������, ͸����, �Է���
	spheres.push_back(Sphere(Vec_3f(0.0, -10004, -20), 10000, Vec_3f(0.20, 0.20, 0.20), 0, 0.0));
	spheres.push_back(Sphere(Vec_3f(0.0, 0, -20), 4, Vec_3f(1.00, 0.32, 0.36), 1, 0.5));
	spheres.push_back(Sphere(Vec_3f(5.0, -1, -15), 2, Vec_3f(0.90, 0.76, 0.46), 1, 0.0));
	spheres.push_back(Sphere(Vec_3f(5.0, 0, -25), 3, Vec_3f(0.65, 0.77, 0.97), 1, 0.0));
	spheres.push_back(Sphere(Vec_3f(-5.5, 0, -15), 3, Vec_3f(0.90, 0.90, 0.90), 1, 0.0));
	//Light
	//��Դ
	spheres.push_back(Sphere(Vec_3f(0.0, 20, -30), 3, Vec_3f(0.0, 0.0, 0.0), 0, 0.0, Vec_3f(1)));
	render(spheres);

	return 0;
}
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
//定义Vec_3类，用于光线追踪
template<typename T>
class Vec_3
{
public:
	//包含x,y,z三个分量
	T x, y, z;
	//构造函数
	Vec_3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec_3(T xx) : x(xx), y(xx), z(xx) {}
	Vec_3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	//定义 * 操作
	Vec_3<T> operator * (const T& f) const {
		return Vec_3<T>(x * f, y * f, z * f);
	}
	Vec_3<T> operator * (const Vec_3<T>& v) const {
		return Vec_3<T>(x * v.x, y * v.y, z * v.z);
	}
	//定义向量点乘
	T dot(const Vec_3<T>& v) const {
		return x * v.x + y * v.y + z * v.z;
	}
	//定义 - 操作
	Vec_3<T> operator - (const Vec_3<T>& v) const {
		return Vec_3<T>(x - v.x, y - v.y, z - v.z);
	}
	//定义 + 操作
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
	//返回向量的长度
	T length2() const {
		return x * x + y * y + z * z;
	}
	T length() const {
		return sqrt(length2());
	}
	//对向量进行归一化
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
	//输出向量的信息
	friend std::ostream& operator << (std::ostream& os, const Vec_3<T>& v) {
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};

typedef Vec_3<float> Vec_3f;

//定义球体类
class Sphere
{
public:
	Vec_3f center; //球心
	float radius, radius2; //球半径、球半径平方
	Vec_3f surfaceColor, emissionColor; //球表面颜色、球自发光颜色(光源)
	float transparency, reflection; //球的透射度、反射度
	//球的构造函数
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
	//使用几何方式, 计算光线与球的交点, 结果保存至t0, t1
	bool intersect(const Vec_3f& rayorigin, const Vec_3f& raydirection, float& t0, float& t1) const
	{
		//计算光线起点到球心中心的向量
		Vec_3f l = center - rayorigin;
		//光线的方向与球的位置相反,不相交
		float tca = l.dot(raydirection);
		if (tca < 0) return false;
		//计算光线与球心的最短距离
		float dist = l.dot(l) - tca * tca;
		//距离大于半径,不相交
		if (dist > radius2) return false;
		float thc = sqrt(radius2 - dist);
		//t0:第一个相交长度; t1:第二个相交长度
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}
};

Vec_3f hitplane_normal;
//定义正方体
class Cube
{
public:
	Vec_3f center; //中心
	float half_length; //半棱长
	Vec_3f cube_normal;
	Vec_3f surfaceColor, emissionColor; //正方体表面颜色、正方体自发光颜色(光源)
	float transparency, reflection; //正方体的透射度、反射度
	//正方体的构造函数
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
	//使用几何方式, 计算光线与正方体的交点, 结果保存至t0
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

//光线追踪函数,输入光线的起点、方向、场景中的球体、光线追踪的最大深度
Vec_3f trace(
	const Vec_3f& rayorigin,
	const Vec_3f& raydirection,
	const std::vector<Sphere>& spheres,
	const std::vector<Cube>& cubes,
	const int& depth
)
{
	int issphere = 1; //表示最近相交的物体是不是球体
	float tnear = INFINITY; //最近的相交参数
	const Sphere* sphere = NULL; //最近的相交球体
	const Cube* cube = NULL; //最近的相交球体
	Vec_3f new_hitpoint_normal = 0; //计算交面的法向量

	//与场景中的物体求交,找到最近的交点
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
		//光线不与场景中的物体相交,返回背景颜色,白色
		if (!sphere) return Vec_3f(2);

		Vec_3f surfaceColor = 0; //定义表面的颜色
		Vec_3f phit = rayorigin + raydirection * tnear; //计算交点
		Vec_3f nhit = phit - sphere->center; //计算交点指向球心的向量nhit,为表面法向
		nhit.normal(); //正则化
		//nhit与光线的点积为正,说明光线起点位于球中心
		float bias = 1e-4;
		bool inside = false;
		if (raydirection.dot(nhit) > 0)
		{
			nhit = -nhit;
			inside = true;
		}
		//处理光线的反射与折射, 计算表面颜色
		if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_DEPTH)
		{
			float facingratio = -raydirection.dot(nhit);
			float fresneleffect = pow(1 - facingratio, 3) * 0.9 + 1 * 0.1;
			//计算反射方向
			Vec_3f reflect_direction = raydirection - nhit * 2 * raydirection.dot(nhit);
			reflect_direction.normal();
			//递归调用光线跟踪函数
			Vec_3f next_reflection = trace(phit + nhit * bias, reflect_direction, spheres, cubes, depth + 1);
			Vec_3f next_refraction = 0;
			//只有当球体有透明效果，才计算透射
			if (sphere->transparency)
			{
				//判断透射的内侧还是外侧
				float ior = 1.1, eta = (inside) ? ior : 1 / ior;
				//计算透射的角度
				float cosi = -nhit.dot(raydirection);
				float k = 1 - eta * eta * (1 - cosi * cosi);
				Vec_3f refraction_direction = raydirection * eta + nhit * (eta * cosi - sqrt(k));
				refraction_direction.normal();
				//递归调用光线跟踪函数
				next_refraction = trace(phit - nhit * bias, refraction_direction, spheres, cubes, depth + 1);
			}
			//表面的颜色是反射和设射颜色的叠加, surfaceColor决定颜色传播的比例
			surfaceColor = (next_reflection * fresneleffect + next_refraction * (1 - fresneleffect) * sphere->transparency) * sphere->surfaceColor;
		}
		//表面反射度和投射度为0,不需要光线追踪;达到最大递归深度,终止光线跟踪
		else {
			for (unsigned i = 0; i < spheres.size(); i++)
			{
				//找到充当光源的球体
				if (sphere[i].emissionColor.x > 0)
				{
					Vec_3f transmission = 1;
					//计算光线方向
					Vec_3f lightDirection = sphere[i].center - phit;
					lightDirection.normal();
					//计算光源与交点间是否有其他物体,添加阴影效果
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
					//叠加物体的表面颜色
					surfaceColor += sphere->surfaceColor * transmission * std::max(float(0), nhit.dot(lightDirection)) * spheres[i].emissionColor;
				}
			}
		}
		// 返回最终的表面颜色
		return surfaceColor + sphere->emissionColor;
	}
	else {
		Vec_3f surfaceColor = 0; //定义表面的颜色
		Vec_3f phit = rayorigin + raydirection * tnear; //计算交点
		Vec_3f nhit = new_hitpoint_normal; //表面法向
		nhit.normal(); //正则化
		//nhit与光线的点积为正,说明光线起点位于中心
		float bias = 1e-4;
		bool inside = false;
		if (raydirection.dot(nhit) > 0)
		{
			nhit = -nhit;
			inside = true;
		}
		//处理光线的反射与折射, 计算表面颜色
		if ((cube->transparency > 0 || cube->reflection > 0) && depth < MAX_DEPTH)
		{
			float facingratio = -raydirection.dot(nhit);
			float fresneleffect = pow(1 - facingratio, 3) * 0.9 + 1 * 0.1;
			//计算反射方向
			Vec_3f reflect_direction = raydirection - nhit * 2 * raydirection.dot(nhit);
			reflect_direction.normal();
			//递归调用光线跟踪函数
			Vec_3f next_reflection = trace(phit + nhit * bias, reflect_direction, spheres, cubes, depth + 1);
			Vec_3f next_refraction = 0;
			//只有当球体有透明效果，才计算透射
			if (cube->transparency)
			{
				//判断透射的内侧还是外侧
				float ior = 1.1, eta = (inside) ? ior : 1 / ior;
				//计算透射的角度
				float cosi = -nhit.dot(raydirection);
				float k = 1 - eta * eta * (1 - cosi * cosi);
				Vec_3f refraction_direction = raydirection * eta + nhit * (eta * cosi - sqrt(k));
				refraction_direction.normal();
				//递归调用光线跟踪函数
				next_refraction = trace(phit - nhit * bias, refraction_direction, spheres, cubes, depth + 1);
			}
			//表面的颜色是反射和折射颜色的叠加, surfaceColor决定颜色传播的比例
			surfaceColor = (next_reflection * fresneleffect + next_refraction * (1 - fresneleffect) * cube->transparency) * cube->surfaceColor;
		}
		//表面反射度和投射度为0,不需要光线追踪;达到最大递归深度,终止光线跟踪
		else {
			for (unsigned i = 0; i < spheres.size(); i++)
			{
				//找到充当光源的球体
				if (spheres[i].emissionColor.x > 0)
				{
					Vec_3f transmission = 1;
					//计算光线方向
					Vec_3f lightDirection = spheres[i].center - phit;
					lightDirection.normal();
					//计算光源与交点间是否有其他物体,添加阴影效果
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
					//叠加物体的表面颜色
					surfaceColor += cube->surfaceColor * transmission * std::max(float(0), nhit.dot(lightDirection)) * spheres[i].emissionColor;
				}
			}
		}
		// 返回最终的表面颜色
		return surfaceColor + cube->emissionColor;
	}
}

//渲染函数,计算每一个像素的颜色
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
	//对所有的像素进行光线追踪
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
	//保存渲染的最终结果
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

// 创建场景, 包括5个球体、1个光源(光源也是球体), 渲染最终的结果
int main(int argc, char* argv[])
{
	std::vector<Sphere> spheres;
	std::vector<Cube> cubes;
	// argument: position, radius, surfacecolor, reflectivity, transparency, emissionColor 
	// 位置, 半径, 表面颜色, 反射率, 透射率, 自发光

	// Cube: 中心，半棱长，表面颜色，一个面的法向量，反射率，折射率，自发光
	spheres.push_back(Sphere(Vec_3f(0.0, -10004, -20), 10000, Vec_3f(0.20, 0.20, 0.20), 0, 0.0));
	spheres.push_back(Sphere(Vec_3f(0.0, 0, -20), 4, Vec_3f(1.00, 0.32, 0.36), 1, 0.5));
	spheres.push_back(Sphere(Vec_3f(5.0, -1, -15), 2, Vec_3f(0.90, 0.76, 0.46), 1, 0.0));
	spheres.push_back(Sphere(Vec_3f(5.0, 0, -25), 3, Vec_3f(0.65, 0.77, 0.97), 1, 0.0));
	spheres.push_back(Sphere(Vec_3f(-5.5, 0, -15), 3, Vec_3f(0.90, 0.90, 0.90), 1, 0.0));
	cubes.push_back(Cube(Vec_3f(10, 0, -20), 2, Vec_3f(0.0, 0.0, 0.1), Vec_3f(0.0, 0.0, 1.0), 1, 0.0));
	//Light
	//光源
	spheres.push_back(Sphere(Vec_3f(0.0, 20, -30), 3, Vec_3f(0.0, 0.0, 0.0), 0, 0.0, Vec_3f(1)));
	render(spheres, cubes);

	return 0;
}
/*
//正方体的构造函数
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
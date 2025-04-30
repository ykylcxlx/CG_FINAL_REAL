#include "myglwidget.h"
#include <GL/glew.h>
#include <algorithm>

MyGLWidget::MyGLWidget(QWidget *parent)
	:QOpenGLWidget(parent)
{
}

MyGLWidget::~MyGLWidget()
{
	delete[] render_buffer;
	delete[] temp_render_buffer;
	delete[] temp_z_buffer;
	delete[] z_buffer;
}

void MyGLWidget::resizeBuffer(int newW, int newH) {
	delete[] render_buffer;
	delete[] temp_render_buffer;
	delete[] temp_z_buffer;
	delete[] z_buffer;
	WindowSizeW = newW;
	WindowSizeH = newH;
	render_buffer = new vec3[WindowSizeH*WindowSizeW];
	temp_render_buffer = new vec3[WindowSizeH*WindowSizeW];
	temp_z_buffer = new float[WindowSizeH*WindowSizeW];
	z_buffer = new float[WindowSizeH*WindowSizeW];
}

void MyGLWidget::initializeGL()
{
	WindowSizeW = width();
	WindowSizeH = height();
	glViewport(0, 0, WindowSizeW, WindowSizeH);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_DEPTH_TEST);
	offset = vec2(WindowSizeH / 2, WindowSizeW / 2);
	// 对定义的数组初始化
	render_buffer = new vec3[WindowSizeH*WindowSizeW];
	temp_render_buffer = new vec3[WindowSizeH*WindowSizeW];
	temp_z_buffer = new float[WindowSizeH*WindowSizeW];
	z_buffer = new float[WindowSizeH*WindowSizeW];
	for (int i = 0; i < WindowSizeH*WindowSizeW; i++) {
		render_buffer[i] = vec3(0, 0, 0);
		temp_render_buffer[i] = vec3(0, 0, 0);
		temp_z_buffer[i] = MAX_Z_BUFFER;			
		z_buffer[i] = MAX_Z_BUFFER;
	}
}

void MyGLWidget::keyPressEvent(QKeyEvent *e) {
	
	switch (e->key()) {
		case Qt::Key_0: scene_id = 0;update(); break;
		case Qt::Key_1: scene_id = 1;update(); break;
		case Qt::Key_9: degree += 35;update(); break;
	}
}

void MyGLWidget::paintGL()
{
	switch (scene_id) {
		case 0:scene_0(); break;
		case 1:scene_1(); break;
	}
}
void MyGLWidget::clearBuffer(vec3* now_buffer) {
	for (int i = 0; i < WindowSizeH*WindowSizeW; i++) {
		now_buffer[i] = vec3(0,0,0);
	}
}

void MyGLWidget::clearBuffer(int* now_buffer) {
	memset(now_buffer, 0, WindowSizeW * WindowSizeH * sizeof(int));
}


void MyGLWidget::clearZBuffer(float* now_buffer) {
	std::fill(now_buffer,now_buffer+WindowSizeW * WindowSizeH, MAX_Z_BUFFER);
}


// 窗口大小变动后，需要重新生成render_buffer等数组
void MyGLWidget::resizeGL(int w, int h)
{
	resizeBuffer(w, h);
	offset = vec2(WindowSizeH / 2, WindowSizeW / 2);
	clearBuffer(render_buffer);
}

void MyGLWidget::scene_0()
{
	//glEnable(GL_DEPTH_TEST);
	// 选择要加载的model
	objModel.loadModel("./objs/singleTriangle.obj");

	// 自主设置变换矩阵
	camPosition = vec3(100 * sin(degree * 3.14 / 180.0) + objModel.centralPoint.y, 100 * cos(degree * 3.14 / 180.0) + objModel.centralPoint.x, 10+ objModel.centralPoint.z);
	camLookAt = objModel.centralPoint;     // 例如，看向物体中心
	camUp = vec3(0, 1, 0);         // 上方向向量
	projMatrix = glm::perspective(radians(20.0f), 1.0f, 0.1f, 2000.0f);

	// 单一点光源，可以改为数组实现多光源
	lightPosition = objModel.centralPoint + vec3(0,100,100);
	clearBuffer(render_buffer);
	clearZBuffer(z_buffer);
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < objModel.triangleCount; i++) {
		Triangle nowTriangle = objModel.getTriangleByID(i);
		drawTriangle(nowTriangle);
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "time:" << duration.count() << std::endl;
	glClear(GL_COLOR_BUFFER_BIT);
	renderWithTexture(render_buffer,WindowSizeH,WindowSizeW);
}


void MyGLWidget::scene_1()
{
	//glEnable(GL_DEPTH_TEST);
	// 选择要加载的model
	objModel.loadModel("./objs/teapot_600.obj");
	//objModel.loadModel("./objs/teapot_8000.obj");
	//objModel.loadModel("./objs/rock.obj");
	//objModel.loadModel("./objs/cube.obj");
	//objModel.loadModel("./objs/singleTriangle.obj");
	
	// 自主设置变换矩阵
	camPosition = vec3(100 * sin(degree * 3.14 / 180.0) + objModel.centralPoint.y, 100 * cos(degree * 3.14 / 180.0) + objModel.centralPoint.x, 10+ objModel.centralPoint.z);
	camLookAt = objModel.centralPoint;     // 例如，看向物体中心
	camUp = vec3(0, 1, 0);         // 上方向向量
	projMatrix = glm::perspective(radians(20.0f), 1.0f, 0.1f, 2000.0f);

	// 单一点光源，可以改为数组实现多光源
	lightPosition = objModel.centralPoint + vec3(0,100,100);
	clearBuffer(render_buffer);
	clearZBuffer(z_buffer);
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < objModel.triangleCount; i++) {
		Triangle nowTriangle = objModel.getTriangleByID(i);
		drawTriangle(nowTriangle);
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "time:" << duration.count() << std::endl;
	glClear(GL_COLOR_BUFFER_BIT);
	renderWithTexture(render_buffer, WindowSizeH, WindowSizeW);
}

void MyGLWidget::drawTriangle(Triangle triangle) {
	// 三维顶点映射到二维平面
	vec3* vertices = triangle.triangleVertices;
	vec3* normals = triangle.triangleNormals;
	FragmentAttr transformedVertices[3];
	clearBuffer(this->temp_render_buffer);
	clearZBuffer(this->temp_z_buffer);
	mat4 viewMatrix = glm::lookAt(camPosition, camLookAt, camUp);

    for (int i = 0; i < 3; ++i) {
		vec4 ver_mv = viewMatrix * vec4(vertices[i], 1.0f);
		float nowz = glm::length(camPosition - vec3(ver_mv));
		vec4 ver_proj = projMatrix * ver_mv;
		transformedVertices[i].x = ver_proj.x + offset.x;
		transformedVertices[i].y = ver_proj.y + offset.y;
		transformedVertices[i].z = nowz;
		transformedVertices[i].pos_mv = ver_mv;  
		mat3 normalMatrix = mat3(viewMatrix);
		vec3 normal_mv = normalMatrix * normals[i];
		transformedVertices[i].normal = normal_mv;
    }

	// 将当前三角形渲染在temp_buffer中
		
	// HomeWork: 1、绘制三角形三边
	//bresenham(transformedVertices[0], transformedVertices[1], 1);
 //   bresenham(transformedVertices[1], transformedVertices[2], 2);
 //   bresenham(transformedVertices[2], transformedVertices[0], 3);

	
	dda(transformedVertices[0], transformedVertices[1], 1);
	dda(transformedVertices[1], transformedVertices[2], 2);
	dda(transformedVertices[2], transformedVertices[0], 3);

    // HomeWork: 2: 用edge-walking填充三角形内部到temp_buffer中
    int firstChangeLine = edge_walking(transformedVertices);

	// 合并temp_buffer 到 render_buffer, 深度测试
	// 从firstChangeLine开始遍历，可以稍快
	for(int h = firstChangeLine; h < WindowSizeH ; h++){
		auto render_row = &render_buffer[h * WindowSizeW];
		auto temp_render_row = &temp_render_buffer[h * WindowSizeW];
		auto z_buffer_row = &z_buffer[h*WindowSizeW];
		auto temp_z_buffer_row = &temp_z_buffer[h*WindowSizeW];
		for (int i = 0 ; i < WindowSizeW ; i++){
			if (z_buffer_row[i] < temp_z_buffer_row[i])
				continue;
			else
			{
				z_buffer_row[i] = temp_z_buffer_row[i];
				render_row[i] = temp_render_row[i];
			}
		}

	}
}


vec3 MyGLWidget::PhoneShading(FragmentAttr& currentPixel) {
	// 定义环境光、光源颜色、材质反射率
	vec3 ambientLight = vec3(0.15, 0.15, 0.15);  // 更改环境光的亮度
	vec3 lightIntensity = vec3(1.0, 1.0, 1.0);   // 光源强度
	vec3 surfaceColor = vec3(0.9, 0.9, 0.9);     // 改变材质的颜色

	// 环境光计算
	vec3 ambientComponent = ambientLight * surfaceColor;

	// 计算漫反射部分
	vec3 normal = normalize(currentPixel.normal);
	vec3 toLight = normalize(lightPosition - vec3(currentPixel.pos_mv));  // 方向向量从片段指向光源
	float diffuseFactor = max(dot(normal, toLight), 0.0f);  // 计算漫反射系数
	vec3 diffuseComponent = diffuseFactor * lightIntensity * surfaceColor;

	// 计算镜面反射部分
	vec3 viewDirection = normalize(-vec3(currentPixel.pos_mv));  // 相机方向
	vec3 reflectionDirection = reflect(-toLight, normal);      // 计算反射方向
	float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32);  // 计算镜面反射系数，光泽度设置为32
	vec3 specularComponent = specularFactor * lightIntensity * surfaceColor;

	// 最终光照强度，累加环境光、漫反射和镜面反射
	vec3 totalLight = ambientComponent + diffuseComponent + specularComponent;

	return totalLight;  // 返回最终的颜色
}


int MyGLWidget::edge_walking(FragmentAttr transformedVertices[3]) {
	float minY = std::min({ transformedVertices[0].y, transformedVertices[1].y, transformedVertices[2].y });
	float maxY = std::max({ transformedVertices[0].y, transformedVertices[1].y, transformedVertices[2].y });

	// 遍历每一条扫描线(行)
	for (float y = std::ceil(minY); y <= std::floor(maxY); y++) {
		std::vector<FragmentAttr> intersections;
		//存储所有交点，每两个点代表一条扫描线的起点和终点


		// 遍历每条边，找到与扫描线的交点
		for (int i = 0; i < 3; i++) {
			FragmentAttr& a = transformedVertices[i];
			FragmentAttr& b = transformedVertices[(i + 1) % 3];

			// 判断扫描线是否与边相交
			if ((y >= a.y && y < b.y) || (y >= b.y && y < a.y)) {
				// 计算交点的x坐标
				float x_intersection = a.x + (y - a.y) * (b.x - a.x) / (b.y - a.y);

				// 使用插值函数获取交点位置的属性
				FragmentAttr interpolated = getLinearInterpolation(a, b, static_cast<int>(x_intersection));

				// 记录交点
				intersections.push_back(interpolated);
			}
		}

		// 排序交点:排序交点的 x 坐标。否则，交点的顺序可能会导致错误的填充顺序。在填充扫描线之间的区域时，需要从左到右的顺序填充像素。如果交点没有排序，可能会导致错误的填充或覆盖。
		std::sort(intersections.begin(), intersections.end(), [](const FragmentAttr& a, const FragmentAttr& b) {
			return a.x < b.x;  // 按x坐标排序
		});

		// 填充扫描线之间的区域
		for (size_t i = 0; i < intersections.size(); i += 2) {
			float xStart = std::floor(intersections[i].x);
			float xEnd = std::floor(intersections[i + 1].x);

			// 填充扫描线之间的像素并设置深度值
			for (float x = xStart; x <= xEnd; x++) {
				// Homework3:在 edge-walking 中， 计算三角形内部的每个片段的着色
				FragmentAttr interpolated = getLinearInterpolation(intersections[i], intersections[i+1], static_cast<int>(x));
				//vec3 pixelColor = PhoneShading(interpolated);  // 计算最终的光照
				vec3 pixelColor = gouraudShading(interpolated);  // 计算最终的光照
				//vec3 pixelColor = blinnPhongShading(interpolated);  // 计算最终的光照



				// 填充像素颜色
				temp_z_buffer[static_cast<int>(x) * WindowSizeW + static_cast<int>(y)] = interpolated.z;
				temp_render_buffer[static_cast<int>(x) * WindowSizeW + static_cast<int>(y)] = pixelColor;
			}
		}
	}
	return 0;
	
}

void MyGLWidget::bresenham(FragmentAttr& start, FragmentAttr& end, int id) {
	int x1 = start.x;
	int y1 = start.y;
	int x2 = end.x;
	int y2 = end.y;

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = (x1 < x2) ? 1 : -1;  // 水平方向的步进
	int sy = (y1 < y2) ? 1 : -1;  // 垂直方向的步进
	int err = dx - dy;  // 误差变量，用于决定下一步选择

	int totalSteps = max(abs(end.x - start.x), abs(end.y - start.y));
	int step = 0; // 当前步数
	while (true) {
		// 这里可以根据你的需要进行渲染操作
		// 例如，绘制当前像素：drawPixel(x1, y1, id);

		//depth 的计算是为了进行深度插值，即在从三角形的起点到终点的过程中，根据插值来计算每个像素的深度值
		float depth = start.z + (end.z - start.z) * (step / (float)totalSteps);

		// 将像素渲染到temp_render_buffer，并更新temp_z_buffer
		temp_render_buffer[x1 * WindowSizeW + y1] = vec3(255, 255, 0);  // 渲染像素颜色
		temp_z_buffer[x1 * WindowSizeW + y1] = depth;  // 更新深度信息
		 //如果到达终点，退出循环
		if (x1 == x2 && y1 == y2) {
			break;
		}

		int e2 = err * 2;
		if (e2 > -dy) {
			err -= dy;
			x1 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y1 += sy;
		}
	}
}
void MyGLWidget::dda(FragmentAttr& start, FragmentAttr& end, int id) {
	int x1 = start.x;
	int y1 = start.y;
	int x2 = end.x;
	int y2 = end.y;

	// 计算差值
	int dx = x2 - x1;
	int dy = y2 - y1;

	// 计算步长
	int steps = max(abs(dx), abs(dy));

	// 计算每一步的增量
	float xIncrement = dx / float(steps);
	float yIncrement = dy / float(steps);

	// 初始化当前点
	float x = x1;
	float y = y1;

	for (int step = 0; step <= steps; step++) {
		// 将当前坐标渲染到屏幕
		// 深度插值计算
		float depth = start.z + (end.z - start.z) * (step / float(steps));

		// 填充渲染缓冲区和更新深度缓冲区
		temp_render_buffer[static_cast<int>(round(x) * WindowSizeW + round(y))] = vec3(255, 255, 0);  // 渲染像素颜色
		temp_z_buffer[static_cast<int>(round(x) * WindowSizeW + round(y))] = depth;  // 更新深度信息

		// 更新坐标
		x += xIncrement;
		y += yIncrement;
	}
}
vec3 MyGLWidget::gouraudShading(FragmentAttr& currentPixel) {
	// 定义环境光、光源颜色、材质反射率
	vec3 ambientLight = vec3(0.15, 0.15, 0.15);  // 更改环境光的亮度
	vec3 lightIntensity = vec3(1.0, 1.0, 1.0);   // 光源强度
	vec3 surfaceColor = vec3(0.9, 0.9, 0.9);     // 改变材质的颜色

	// 环境光计算
	vec3 ambientComponent = ambientLight * surfaceColor;

	// 计算漫反射部分
	vec3 normal = normalize(currentPixel.normal);
	vec3 toLight = normalize(lightPosition - vec3(currentPixel.pos_mv));  // 方向向量从片段指向光源
	float diffuseFactor = max(dot(normal, toLight), 0.0f);  // 计算漫反射系数
	vec3 diffuseComponent = diffuseFactor * lightIntensity * surfaceColor;

	// 计算镜面反射部分
	vec3 viewDirection = normalize(-vec3(currentPixel.pos_mv));  // 相机方向
	vec3 reflectDirection = reflect(-toLight, normal);          // 计算反射方向
	float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0f), 32);  // 镜面反射
	vec3 specularComponent = specularFactor * lightIntensity * surfaceColor;

	// 将所有光照分量加在一起
	vec3 totalLighting = ambientComponent + diffuseComponent + specularComponent;

	return totalLighting;  // 返回顶点计算出的光照颜色
}

vec3 MyGLWidget::blinnPhongShading(FragmentAttr& currentPixel) {
	// 定义环境光、光源颜色、材质反射率
	vec3 ambientLight = vec3(0.15, 0.15, 0.15);  // 更改环境光的亮度
	vec3 lightIntensity = vec3(1.0, 1.0, 1.0);   // 光源强度
	vec3 surfaceColor = vec3(0.9, 0.9, 0.9);     // 改变材质的颜色

	// 环境光计算
	vec3 ambientComponent = ambientLight * surfaceColor;

	// 计算漫反射部分
	vec3 normal = normalize(currentPixel.normal);
	vec3 toLight = normalize(lightPosition - vec3(currentPixel.pos_mv));  // 方向向量从片段指向光源
	float diffuseFactor = max(dot(normal, toLight), 0.0f);  // 计算漫反射系数
	vec3 diffuseComponent = diffuseFactor * lightIntensity * surfaceColor;

	// 计算 Blinn-Phong 的镜面反射部分
	vec3 viewDirection = normalize(-vec3(currentPixel.pos_mv));  // 相机方向
	vec3 halfwayVector = normalize(toLight + viewDirection);    // 半程向量
	float specularFactor = pow(max(dot(normal, halfwayVector), 0.0f), 32);  // 镜面反射
	vec3 specularComponent = specularFactor * lightIntensity * surfaceColor;

	// 最终光照强度，累加环境光、漫反射和镜面反射
	vec3 totalLight = ambientComponent + diffuseComponent + specularComponent;

	return totalLight;  // 返回最终的颜色
}




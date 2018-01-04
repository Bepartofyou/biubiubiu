/*
 * pngdecoder.c
 *
 *  Created on: 2017年12月28日
 *      Author: mj
 */

#include "pngdecoder.h"

PNGHandle* mallocPngHandle()
{
	PNGHandle* handle = (PNGHandle*) malloc(sizeof(PNGHandle));
	memset(handle, 0, sizeof(PNGHandle));
	return handle;
}

void freePngHandle(PNGHandle* handle)
{
	if(!handle){
		return;
	}

	if(handle->data){
		free(handle->data);
	}

	free(handle);
}

int decodePng(const char* filename, PNGHandle* handle)
{
	FILE* fp;
	unsigned char header[8];     //8
	fp = fopen(filename, "rb");
	if (fp == NULL)
		return -1;

	//读取文件头判断是否所png图片.不是则做出相应处理
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8)) {
		fclose(fp);
		return -1; //每个错误处理都是一样的！这样报错之后锁定就要花点小时间来！
	}

	//根据libpng的libpng-manual.txt的说明使用文档 接下来必须初始化png_structp 和 png_infop
	png_structp png_ptr; //图片
	png_infop info_ptr; //图片的信息

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); //后三个是绑定错误以及警告的函数这里设置为空
	if (!png_ptr) //做出相应到初始化失败的处理
	{
		fclose(fp);
		return -1;
	}
	//根据初始化的png_ptr初始化png_infop
	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		//初始化失败以后销毁png_structp
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(fp);
		return -1;
	}

	//老老实实按照libpng给到的说明稳定步骤来  错误处理！
	if (setjmp(png_jmpbuf(png_ptr))) {
		//释放占用的内存！然后关闭文件返回一个贴图ID此处应该调用一个生成默认贴图返回ID的函数
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(fp);
		return -1;
	}

	//你需要确保是通过2进制打开的文件。通过i/o定制函数png_init_io
	png_init_io(png_ptr, fp);
	//似乎是说要告诉libpng文件从第几个开始missing
	png_set_sig_bytes(png_ptr, 8);
	//如果你只想简单的操作你现在可以实际读取图片信息了！
	png_read_info(png_ptr, info_ptr);

	//获得图片到信息 width height 颜色类型  字节深度
	uint32_t width = png_get_image_width(png_ptr, info_ptr);
	uint32_t height = png_get_image_height(png_ptr, info_ptr);
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	//如果图片带有alpha通道就需要
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	//隔行扫描图片  这个必须要调用才能进行
	int number_of_passes = png_set_interlace_handling(png_ptr);
	//将读取到的信息更新到info_ptr
	png_read_update_info(png_ptr, info_ptr);

	//读文件
	if (setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);
		return -1;
	}

	handle->width = width;
	handle->height = height;
	handle->length = width * height * 4;
	handle->data = (uint8_t*)malloc(sizeof(uint8_t) * handle->length);

	//使用动态数组  设置长度
	png_bytep * row_pointers; //图片的数据内容
	int k;
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);

	for (k = 0; k < height; k++)
		row_pointers[k] = NULL;

	//通过扫描流里面的每一行将得到的数据赋值给动态数组
	for (k = 0; k < height; k++)
		//row_pointers[k] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
		row_pointers[k] = (png_bytep) png_malloc(png_ptr,
				png_get_rowbytes(png_ptr, info_ptr));
	//由于png他的像素是由 左-右-从顶到底 而贴图需要的像素都是从左-右-底到顶的所以在这里需要把像素内容进行一个从新排列
	//读图片
	png_read_image(png_ptr, row_pointers);

	//用于改变png像素排列的问题
	int row, col, pos;
	pos = 0;
	for (row = 0; row < height; row++) {
		memcpy(handle->data + pos, row_pointers[row], 4 * width);
		pos += width * 4;
	}

	fclose(fp);
	return 0;
}
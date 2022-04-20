#include <fs.h>
#define MIN(x,y) ((x)<(y)?(x):(y))

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;//use the two functional pointer to implement VFS
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENT, FD_DISPINFO};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
  [FD_EVENT]  = {"/dev/events", 0, 0, events_read, invalid_write},//dev is a file
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  #include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
	int screen_w = io_read(AM_GPU_CONFIG).width;
	int screen_h = io_read(AM_GPU_CONFIG).height;
	//sscanf(buf, "%[A-Z:]%d %[A-Z:]%d", NULL, &screen_w, NULL, &screen_h);//关照重要点
	file_table[FD_FB].size = screen_w * screen_h * sizeof(uint32_t);
	file_table[FD_FB].open_offset = 0;
	return;
}


int fs_open(const char *pathname, int flags, int mode){
	size_t i = 0;
	size_t file_table_size = sizeof(file_table)/sizeof(Finfo);
	for(; i < file_table_size; i++){
		if(strcmp(pathname, file_table[i].name)==0){
			file_table[i].open_offset = 0;
			return i;
		}
	}
	panic("unvalid filename");
}


size_t fs_read(int fd, void *buf, size_t len){
	if(file_table[fd].read==NULL){
		len = MIN(len, file_table[fd].size - file_table[fd].open_offset);
		assert(len + file_table[fd].open_offset <= file_table[fd].size);

		ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
		file_table[fd].open_offset += len;
	}
	else{
		file_table[fd].read(buf, 0, len);
	}
	return len;
}

size_t fs_write(int fd, const void *buf, size_t len){
	if(file_table[fd].write==NULL){
		len = MIN(len, file_table[fd].size - file_table[fd].open_offset); 
		assert(len + file_table[fd].open_offset <= file_table[fd].size);

		ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
		//普通文件读写函数
		file_table[fd].open_offset += len;
	}
	else{
		if(fd==FD_FB){
			file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
			file_table[fd].open_offset += len;//一行会分为两行写到屏幕上，400像素点会分为256和144，分别在同一行从头写
			                                  //没有这一行则有bug,不知什么原因，很奇怪，害老子查了一天
		}
		else{file_table[fd].write(buf, 0, len);}
	}
	return len;
}



//SEEK_SET: The file offset is set to offset bytes.
//SEEK_CUR: The file offset is set to its current location plus offset bytes.
//SEEK_END: The file offset is set to the size of the file plus offset bytes.
size_t fs_lseek(int fd, size_t offset, int whence){
	switch(whence){
		case SEEK_SET: file_table[fd].open_offset = offset;  break;
		case SEEK_CUR: file_table[fd].open_offset += offset; break;
		case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
		default:
			 panic("unvalid lseek whence");
	}
	return file_table[fd].open_offset;
}


int fs_close(int fd){
	file_table[fd].open_offset = 0;
	return 0;
}


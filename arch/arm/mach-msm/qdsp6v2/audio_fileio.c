#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/syscalls.h>

#include <asm/processor.h>
#include <asm/uaccess.h>

#define EOF	(-1)

struct file *audiofs_fopen(const char *filename, int flags, int mode)
{
	struct file *filp = filp_open(filename, flags, mode);
	return (IS_ERR(filp))? NULL : filp;
}

void audiofs_fremove(const char *filename)
{
	sys_unlink(filename);
}

void audiofs_fclose(struct file *filp)
{
	if(filp)
		fput(filp);
}

int audiofs_fseek(struct file *filp, int offset, int whence)
{
	int pos = filp->f_pos;

	if(filp) {
		if(whence == SEEK_SET)
			pos = offset;
		else if(whence == SEEK_CUR)
			pos += offset;

		if(pos < 0)
			pos = 0;

		return(filp->f_pos = pos);
	} else 
		return -ENOENT;
}

int audiofs_ftell(struct file *filp)
{
	if(filp) {
		return filp->f_pos;
	}

	return -ENOENT;
}

int audiofs_fread(struct file *filp, char *buf, int len)
{
	int readlen;
	mm_segment_t oldfs;

	if(filp == NULL)
		return -ENOENT;
	if(filp->f_op->read == NULL)
		return -ENOSYS;
	if(((filp->f_flags & O_ACCMODE) & O_RDONLY) != 0)
		return -EACCES;
	
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	readlen = filp->f_op->read(filp, buf, len, &filp->f_pos);
	set_fs(oldfs);

	return readlen;
}

int audiofs_fgetc(struct file *filp)
{
	int len;
	unsigned char buf[4];
	
	len = audiofs_fread(filp, (char *) buf, 1);

	if(len > 0)
		return buf[0];
	else if(len == 0)
		return EOF;
	else 
		return len;
}

int panech_fgets(struct file *filp, char *str, int size)
{
	char *cp;
	int len, readlen;
	mm_segment_t oldfs;

	if(filp && filp->f_op->read && ((filp->f_flags & O_ACCMODE) & O_WRONLY) == 0) {
		oldfs = get_fs();
		set_fs(KERNEL_DS);

		for(cp  = str, len = -1, readlen = 0; readlen < size - 1; ++cp, ++readlen) { 

			if((len = filp->f_op->read(filp, cp, 1, &filp->f_pos)) <= 0)
				break;
			if(*cp == '\n') {
				++cp;
				++readlen;
				break;
			}
		}
		*cp = 0;
		set_fs(oldfs);
		return (len < 0 || readlen == 0)? 0 : 1;
	} else
		return 0;
}

int audiofs_fwrite(struct file *filp, char *buf, int len)
{
	int writelen;
	mm_segment_t oldfs;

	if(filp == NULL)
		return -ENOENT;
	if(filp->f_op->write == NULL)
		return -ENOSYS;
	if(((filp->f_flags & O_ACCMODE) & (O_WRONLY | O_RDWR)) == 0)
		return -EACCES;
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	writelen = filp->f_op->write(filp, buf, len, &filp->f_pos);
	set_fs(oldfs);

	return writelen;
}

int audiofs_fputc(struct file *filp, int ch)
{
	int len;
	unsigned char buf[4];

	buf[0] = (unsigned char) ch;
	len = audiofs_fwrite(filp, buf, 1);

	if(len > 0)
		return buf[0];
	else
		return EOF;
}

int audiofs_fputs(struct file *filp, char *str)
{
	return audiofs_fwrite(filp, str, strlen(str));
}

int audiofs_fprintf(struct file *filp, const char *fmt, ...)
{
	static char s_buf[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf(s_buf, fmt, args);
	va_end(args);

	return audiofs_fputs(filp, s_buf);
}



#include "utf.h"
#include "debug.h"
#include "wrappers.h"
#include <unistd.h>

int
from_utf16be_to_utf16le(int infile, int outfile)
{
  int bom;
  utf16_glyph_t buf;
  ssize_t bytes_read;
  ssize_t bytes_to_write;
  int ret = -1;

  bom = UTF16LE;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  reverse_bytes(&bom, 3);
#endif
  write_to_bigendian(outfile, &bom, 2);

  while ((bytes_read = read_to_bigendian(infile, &(buf.upper_bytes), 2)) > 0) {
    bytes_to_write = 4; /* utf-32 future compatibility */
    reverse_bytes(&buf.upper_bytes, 2);
    if(is_lower_surrogate_pair(buf)) {
      if((bytes_read = read_to_bigendian(infile, &buf.lower_bytes, 2) > 0)) {
        break;
      }
      reverse_bytes(&(buf.lower_bytes), 2);
      bytes_to_write += 2;
    }
    write_to_bigendian(outfile, &bom, bytes_to_write);
  }
  ret = bytes_read;
  return ret;
}



int
from_utf16be_to_utf8(int infile, int outfile)
{
  int ret = 0;
  //int bom;
  utf8_glyph_t utf8_buf;
  ssize_t bytes_read = 0;

  //size_t remaining_bytes;
  size_t size_of_glyph;
  code_point_t code_point;
  utf16_glyph_t utf16_buf;

  while((bytes_read = read_to_bigendian(infile, &utf16_buf.upper_bytes, 2))>0){

    reverse_bytes(&utf16_buf.upper_bytes, 2);

    if(is_upper_surrogate_pair(utf16_buf)){
      info("Is Up sp");
      bytes_read = read_to_bigendian(infile, &utf16_buf.lower_bytes, 2);
      reverse_bytes(&utf16_buf.lower_bytes, 2);
      code_point = utf16_glyph_to_code_point(&utf16_buf);
    } else {
      code_point = utf16_buf.upper_bytes;
    }
    //info("UTF16 ub: %x lb: %x", utf16_buf.upper_bytes, utf16_buf.lower_bytes);
    size_of_glyph = utf8_glyph_size_of_code_point(code_point);
    info("code_point: %x", code_point);
    utf8_buf = (get_utf8_encoding_function(size_of_glyph))(code_point);
    //info("utf8_buf %s", utf8_buf);
    write_to_bigendian(outfile, &utf8_buf, size_of_glyph);
  }
  ret = bytes_read;
  return ret;
  //return infile+outfile;
}

utf16_glyph_t
code_point_to_utf16be_glyph(code_point_t code_point, size_t *size_of_glyph)
{
  utf16_glyph_t ret;

  memeset(&ret, 0, sizeof ret);
  if(is_code_point_surrogate(code_point)) {
    debug("Is sp");
    code_point -= 0x10000;
    ret.upper_bytes = (code_point >> 10) + 0xD800;
    ret.lower_bytes = (code_point & 0x3FF) + 0xDC00;
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    reverse_bytes(&ret.upper_bytes, 2);
    reverse_bytes(&ret.lower_bytes, 2);
  #endif
    *size_of_glyph = 4;
  }
  else {
    debug("Not sp");
    ret.upper_bytes |= code_point;
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    reverse_bytes(&ret.upper_bytes, 2);
  #endif
    *size_of_glyph = 2;
  }
  return ret;
}

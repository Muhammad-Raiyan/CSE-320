#include "debug.h"
#include "utf.h"
#include "wrappers.h"
#include <stdlib.h>

int
main(int argc, char *argv[])
{
  int infile, outfile, in_flags, out_flags;
  parse_args(argc, argv);
  debug("Success: parse_args()");
  check_bom();
  debug("Success: check_bom()");
  print_state();
  debug("Success: print_state()");
  in_flags = O_RDONLY;
  out_flags = O_WRONLY | O_CREAT;
  infile = Open(program_state->in_file, in_flags);
  outfile = Open(program_state->out_file, out_flags);
  debug("Successfully Opened in and outfiles");
  lseek(SEEK_SET, program_state->bom_length, infile); /* Discard BOM */
  debug("Success: lseek()");
  get_encoding_function()(infile, outfile);
  debug("Success: get_encoding_function()");
  if(program_state != NULL) {
    close(infile);
  }
  //I think this is how this works
  free((void*)(intptr_t)outfile);
  free((void*)(intptr_t)infile);
  //fclose(program_state->in_file);
  debug("ALL DONE");
  return EXIT_SUCCESS;
}

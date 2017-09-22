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
  in_flags = O_RDONLY;
  out_flags = O_WRONLY | O_CREAT | O_TRUNC;
  infile = Open(program_state->in_file, in_flags);
  outfile = Open(program_state->out_file, out_flags);
  info("open success. Infile: %d Outfile %d in_flags %d out_flags %d", infile, outfile, in_flags, out_flags);
  lseek(SEEK_SET, program_state->bom_length, infile); /* Discard BOM */
  debug("Success: lseek()");
  convertion_func_t trans = get_encoding_function();//(infile, outfile);
  trans(infile, outfile);
  //convertion_func_t = get_encoding_function();

  debug("Success: get_encoding_function()");
  if(program_state != NULL) {
    close(infile);
  }

  freeAll();
  close(infile);
  close(outfile);
  debug("ALL DONE");
  return EXIT_SUCCESS;
}


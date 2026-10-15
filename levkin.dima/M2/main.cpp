#include <iostream>
#include "io.hpp"
#include <vector>

struct DB {
  std::vector< int > processes;
} db;

void cmd_move(DB *, std::istream &is, std::ostream &os)
{
  std::string dummy;
  is >> dummy;
  os << dummy << dummy << "\n";
}

void cmd_rename(DB *, std::istream &is, std::ostream &os)
{
  std::string dummy;
  is >> dummy;
  os << dummy << dummy << "\n";
}
int main()
{
  using IoHandler = levkin::IO< DB >;

  const std::array< IoHandler::cmd_t, 2 > cmds = {{{&cmd_move, "move"}, {&cmd_rename, "rename"}}};

  IoHandler io(std::cin, std::cout, db, cmds);

  io.run("move");

  std::cout << "levkin.dima\n" << sizeof(io);
}

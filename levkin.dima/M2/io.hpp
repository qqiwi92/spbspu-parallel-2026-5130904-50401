#ifndef IO_H
#define IO_H

#include <string_view>
#include <fstream>
#include <array>

namespace levkin {

  template< class DB >
  struct IO {
    using fn_t = void (*)(DB*, std::istream& is, std::ostream& os);
    struct cmd_t {
      fn_t fn;
      const char* name;
    };

    template< std::size_t N >
    IO(std::istream& is, std::ostream& os, DB& db, const std::array< cmd_t, N >& cmds):
      _is(is),
      _os(os),
      _db(std::move(db)),
      _count(N),
      _commands(cmds)
    {}

    void run(char* name)
    {
      for (std::size_t i = 0; i < _count; ++i) {
        if (_commands[i].name == name) {
          _commands[i].fn(&_db, _is, _os);
          return;
        }
      }
    }

  private:
    std::array< cmd_t, 32 > _commands{};
    std::size_t _count = 0;
    std::istream& _is;
    std::ostream& _os;
    DB _db;
  };

}

#endif

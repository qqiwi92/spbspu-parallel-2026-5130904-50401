#ifndef IO_H
#define IO_H

#include <string_view>
#include <fstream>
#include <array>

namespace levkin {

  template< class DB >
  struct IO {
    IO(std::ifstream& is, DB& db):
      _is(is),
      _db(std::move(db))
    {}

    using fn_t = void (*)(DB*, std::ifstream&);

    struct cmd_t {
      fn_t fn;
      std::string_view name;
    };

    template< std::size_t N >
    void fillWithCmd(const std::array< cmd_t, N >& cmds)
    {
      _commands = cmds;
      _count = N;
    }

    void run(std::string_view name)
    {
      for (std::size_t i = 0; i < _count; ++i) {
        if (_commands[i].name == name) {
          _commands[i].fn(nullptr, _is);
          return;
        }
      }
    }

  private:
    std::array< cmd_t, 32 > _commands{};
    std::size_t _count = 0;
    std::ifstream& _is;
    DB _db;
  };

}

#endif

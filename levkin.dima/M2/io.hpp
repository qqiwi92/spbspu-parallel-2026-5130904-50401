#ifndef IO_HPP
#define IO_HPP

#include <array>
#include <cstddef>
#include <cstring>
#include <istream>
#include <ostream>

namespace levkin {

  template< class DB >
  struct IO {
    using fn_t = void (*)(DB *, std::istream &is, std::ostream &os);
    struct cmd_t {
      fn_t fn;
      const char *name;
    };

    template< std::size_t N >
    IO(std::istream &is, std::ostream &os, DB &db, const std::array< cmd_t, N > &cmds):
      _is(is),
      _os(os),
      _db(db),
      _count(N),
      _commands{}
    {
      for (std::size_t i = 0; i < N; ++i) {
        _commands[i] = cmds[i];
      }
    }

    void run(const char *name)
    {
      for (std::size_t i = 0; i < _count; ++i) {
        if (_commands[i].name && name && std::strcmp(_commands[i].name, name) == 0) {
          _commands[i].fn(&_db, _is, _os);
          return;
        }
      }
    }

  private:
    std::istream &_is;
    std::ostream &_os;
    DB &_db;
    std::size_t _count = 0;
    std::array< cmd_t, 32 > _commands{};
  };

}

#endif

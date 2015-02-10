cat << EOF > include/$1.hh
#ifndef H_$1
#define H_$1

namespace Fed9U {

  /**
   * \class $1
   * \brief ###BRIEF###DESCRIPTION###
   * \author Jonathan Fulcher
   * \author Emlyn Corrin
   * \version 0.0
   * \date `date  +"%d %b %Y"`
   *
   * ###LONG###DESCRIPTION###
   ***********************************************/

  class $1 {
  public:
    $1();
  protected:
  private:
  };

}

#endif // H_$1
EOF
cat << EOF > src/$1.cc
#include "$1.hh"

namespace Fed9U {

}
EOF
emacs include/$1.hh src/$1.cc &

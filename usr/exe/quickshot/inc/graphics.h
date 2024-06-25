// Ascii Art Graphics Header File // ~ eylon

#if !defined(__GRAPHICS_H)
#define __GRAPHICS_H

const char* shot_str = "\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v(\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v_\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v,\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v)\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v/=>\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v\\  :  /\v\v\v\v\v\v\v\v\v\v\v\v\v(  +____________________/\\/\\___ / /|\n\
\v\v\v\v\v\v\v\v\v\v`. __/ \\__ .'\v\v\v\v\v\v\v\v\v\v.''._____________'._____      / /|/\\\n\
\v\v\v\v\v\v\v\v\v\v_ _\\     /_ _\v\v\v\v\v\v\v\v\v: () :              :\\ ----\\|    \\ )\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v/_   _\\\v\v\v\v\v\v\v\v\v\v\v\v\v'..'______________.'0|----|      \\\n\
\v\v\v\v\v\v\v\v\v\v\v.'  \\ /  `.\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v0_0/____/        \\\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v/  :  \\\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v|----    /----\\\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v'\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v|| -\\\\ --|      \\\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v||   || ||\\      \\\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\\\\____// '|      \\\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v.'/       |\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v.:/        |\n\
\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v\v:/_________|\n";


const char* enemies_str[] = {
"\n\n            _---_\n\
           ###  '\\\n\
           ##    )\n\
            -----_________===='\n\
          (      -------- /'\n\
         /|      |\n\
        /  |    |\n\
         \\ |    |\n\
           |     -_____\n\
           \\    _____  \\\n\
            |  /      | |\n\
     ______/  /        | |__\n\
    \\ _______/          |___)\n",

"\n\n\n\n                                         \"\"\"\"\"\"\"\n\
                                         |\"\"\"\"\"\"\n\
                           _________     O   C\"\"\n\
                          //-------\\   /_     \\\n\
  L______________||O-----------------\\   |____/_\\ /\n\
  -----------------||||| =========== /\\ ____\\    v|\\\n\
                    OOOO-L______________          /\n\
                      --____||||___\\<\n\
                            ||||    (|-- |\n\
                            ----\n",

"\n          ----.\n\
         \"   _}\n\
         \"@   >\n\
         |\\   7\n\
         / `-- _         ,-------,****\n\
      ~    >o<  \\---------o{___}-\n\
     /  |  \\  /  ________/8'\n\
     |  |       /         \"\n\
     |  /      |",

"\n\n             ,-.\n\
     \"===o\\_(-_-)\n\
          '--.   \\\n\
              )___\\\n\
            ,-;  .'\n\
            \\/ ,--.,\n\
            [__.-']/\n\
             ~'\n"
};

#endif
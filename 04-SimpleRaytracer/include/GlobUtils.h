#ifndef GLOBUTILS_H
#define GLOBUTILS_H

#define DISABLE_COPY(Classname) \
    Classname(const Classname&) = delete; \
    Classname& operator=(Classname&) = delete;

#define DISABLE_COPY_AND_MOVE(Classname) \
    Classname(const Classname&) = delete; \
    Classname(const Classname&&) = delete; \
    Classname& operator=(Classname&) = delete; \
    Classname& operator=(Classname&&) = delete;

#endif // GLOBUTILS_H
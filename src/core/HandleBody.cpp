#include "bellezasys/core/HandleBody.hpp"

namespace bellezasys {

// definicao dos contadores globais declarados em HandleBody.hpp
#ifdef DEBUGING
int numHandleCreated = 0;
int numHandleDeleted = 0;
int numBodyCreated = 0;
int numBodyDeleted = 0;
#endif

Body::Body()
{
#ifdef DEBUGING
    numBodyCreated++;
#endif
}

void Body::attach()
{
    ++refCount_;
}

void Body::detach()
{
    --refCount_;
    if (refCount_ == 0) {
#ifdef DEBUGING
        numBodyDeleted++;
#endif
        delete this;
    }
}

int Body::refCount() const
{
    return refCount_;
}

Body::~Body() = default;

} // namespace bellezasys

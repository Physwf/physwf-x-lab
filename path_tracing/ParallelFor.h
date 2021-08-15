#include "ThreadPool.h"
#include "Vector.h"

void ParallelFor(std::function<void(size_t)> func, size_t Count, size_t chunkSize);
void ParallelFor2D(std::function<void(Vector2i)> func, Vector2i count);
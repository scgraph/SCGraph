#ifndef SCGRAPH_BUFFER_POOL_HH
#define SCGRAPH_BUFFER_POOL_HH

#include "boost/shared_ptr.hpp"
#include <vector>
#include <string>

struct Buffer
{
	unsigned char *_data;
};

class BufferPool
{
	std::vector <boost::shared_ptr<Buffer> > _buffers;

	public:
		void b_alloc (int buffer_number, int number_of_frames = 0, int number_of_channels = 1);

		void b_alloc_read (int buffer_number, std::string file_name, int starting_frame = 0, int number_of_frames = 0);
};

#endif

# VoxelOctree-1
  This is a what I've decided to call a "Voxel Octree Raytracer". It generate a world made of blocks which you can move around in and view using a ray traced image. It uses OpenGL compute shaders and GLFW. I started by copying code from: https://antongerdelan.net/opengl/compute.html
just to get the compute shaders working.
Features:
-5 different blocks including mirrrors.
-Sun shadow
-Diffuse lighting
-Temporal Filtering
  The world is stored in an "octree". Its not truly an octree though. Really the world is made of cubic "chunks" 32 blocks in width, making each chunk hold about 32k blocks. The chunk is stored in a 3D array with 1 byte for each block (0 for air non-zero for any other block). The 3 least significant bits for each byte hold the block type and remaining 5 bits are used for the "octree". The occupancy of an octree node is stored in a single bit.
 An octree has "nodes" which are the cubic regions that an octree is subdivided into. I will say that the "level" of a node is 5 if that nodes width is 2^5=32 (i.e an entire chunk). I will say the level is 0 if the width of that node is 2^0=1 (i.e a single block)  
  For example if our position is (23,6,19) and we want to check if the 16x16x16 cube around us is occupied we do the following.
  Round our coordinates down to multiples of 16 so (16,0,16).
  Consider the size of the cube we are checking. It is 16 blocks wide so it is a level 4 node.
  Go the byte at (16,0,16) and go to 4th most significant bit. (this is one of the corners of our 16x16x16 node) 
  That bit will be 1 if the 16x16x16 region around them is occupied and 0 otherwise.
 If a ray wants to check the occupancy of an "octree" node which would be a cube with dimensions 2^n where n is the "level" of that node. Level 5 would mean the whole chunk (2^5 blocks in width) and level 0 would mean a single block. The ray just specifies the level of the node that it would like to check before stepping through that node.
  I did it this because it made it easier to program but it is far from making efficient use of the memory. I can already think of a way which I could reduce the size of a chunk by 25% and still add another bit to the block types meaning I could have up to 16 types of blocks(including air) instead of the measly 8 block types possible with my current system.

all:
	rm -rf build; mkdir build; cd build; cmake -DALL=ON ..; make; mv l2p1 ..; mv l2p2 ..; mv l2p3 ..

l2p1:
	rm -rf build; mkdir build; cd build; cmake -DPART1=ON -DALL=OFF ..; make; mv l2p1 ..

l2p2:
	rm -rf build; mkdir build; cd build; cmake -DPART2=ON -DALL=OFF ..; make; mv l2p2 ..

l2p3:
	rm -rf build; mkdir build; cd build; cmake -DPART3=ON -DALL=OFF ..; make; mv l2p3 ..

clean:
	rm famtree; rm -rf build;

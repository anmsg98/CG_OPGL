=========설명========
0. 명령어 ->	slice[ mouse_drag ]   LINE/FILL[ tab ]   speed[ 1 , 2 , 3 ]   
		path[ ` ]   exit[ Q ]

1. 슬라이스 ->	

2. 마우스 -> 	드래그 마다 사각형을 생성, 이를 std::vector 로 저장해둔뒤
		begin , end 와의 거리에 따라 크기가 변하도록 하여
		붓글씨 효과을 내려고 했음.
		시간이 지나면 처음 생성관 사각형부터 차례로 삭제됨.
		여러개의 붓글씨를 중복하여 그릴 수 있음.



======구성 파일 목록=====
vertex_zero.txt
frag_zero.txt
SFP.cpp

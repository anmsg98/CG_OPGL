/// 낙서장 하고싶은 말이나 혼잣말 혹은 아이디어 적어두기

링 통과 체크포인트 착륙까지, 

9분할 중심점에서 모서리 도달시 반대편에서 출몰. 무한맵



이미지 훔쳐오는 링크 https://thefiveplanets.org/b01/data/graphics/
이미지 변환 링크 http://convert-my-image.com/ImageConverter

LoadTexture() 함수의 마지막 인자 (channel) 은 이미지 파일 마다 다르다!
윈도우 탐색기에서 파일에 우클릭 ->속성 ->디테일(자세히) ->Bit depth 를 확인!
Bit depth 는 항상 8의 배수. 나누기 8 한 값을 채널인자로 넣어주면 됩니다. (1-4 까지 가능 그 밖의 이미지가 있으면 알려주세요)

좌표축 당 각의 회전이 아닌 사원수회전을 이용하는 이유 
https://m.blog.naver.com/PostView.nhn?blogId=jsjhahi&logNo=205951840&proxyReferer=https:%2F%2Fwww.google.com%2F

개인적인 목표: 
버텍스,프래그먼트 하나씩의 셰이더만을 사용하기
광원 여러개를 활용, 기상효과 보이기 (비, 번개, 천둥, 해, 달)

다음에 할일 : 충돌쳌, 번개, 비 ,소리

소리
git
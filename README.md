## Smiple ray trace demo


### Already done

- [x] 点光源+硬阴影
- [x] 光源直接照明角度计算
- [x] 物体反射材质
- [x] 物体折射材质
- [x] 添加球体支持
- [x] 可移动的摄像机
- [x] 添加plane支持
- [x] 添加漫反射材质支持 
- [x] 抗锯齿
- [x] 普通Plane
- [x] Plane棋盘格
- [x] 聚光灯光源
- [x] 添加体积光源支持
- [x] 添加软阴影支持
- [x] 修正漫反射对于间接来源反射光的不精确模拟 ???


### Need to do

- [ ] 添加三角形片支持
- [ ] 添加三角形拼接3D物体
- [ ] 添加贴图支持
- [ ] GPU?
- [ ] 增加光线物体碰撞预筛选（AABB+规则网格或八叉树）
- [ ] 合并漫反射模拟和普通Phong光照模型
- [ ] 物体表面漫反射抗锯齿


### Bug already know
- [x] ~~摄像机转向不正常~~
- [x] ~~Plane在降低漫反射系数后出现噪点~~
- [x] ~~Plane不正常的过曝导致反射丢失~~
- [ ] 表面漫反射蒙特卡洛采样不正常，暂时关闭(已定位BUG，在新分支中解决)


### Always
- [ ] 性能优化



### Demo Scene:

![Demo](http://121.49.97.197:10101/maozi/RTXmaomaozi/raw/f997811963cadd276eed63c174fb16c7cde13810/demo.png "Demo")

![Demo2](http://121.49.97.197:10101/maozi/RTXmaomaozi/raw/f997811963cadd276eed63c174fb16c7cde13810/demo2.png "Demo2")

#### 漫反射原始着色图
![Demo3](http://121.49.97.197:10101/maozi/RTXmaomaozi/raw/e3fb27ad364fb4a8c2dca448c7f2b781c32056c3/%E6%97%A0%E6%A0%87%E9%A2%98.png "Demo3")

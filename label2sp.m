
%for i=1:size(samples,2);
   id = 'Mossskas141';
   im = imread(sprintf('./LBtool/img/%s.jpg.png',id));
   dsp  = double(im(:,:,1)) + double(im(:,:,2))*256 +double(im(:,:,3))*256*256; 
   vis = zeros(1,256*256);
   
   fid = fopen(sprintf('./LBtool/img/%s.jpg.dat',id),'r');
   val = textscan(fid,'%d%d');
   fclose(fid);
   for i=1:size(val{1},1)
       vis(val{1}(i)) = val{2}(i);
   end
   sim = uint8(zeros(size(im,1),size(im,2),3));
   for i=1:size(sim,1)
       for j=1:size(sim,2)
           switch vis(dsp(i,j))
               case 0
                    sim(i,j,:) = [255,255,255];
                case 1
                    sim(i,j,:) = [0,0,255];
                case 2
                    sim(i,j,:) = [255,0,0];
                case 3
                    sim(i,j,:) = [0,255,0];
                case 4
                    sim(i,j,:) = [255,255,0];
           end
       end
   end
    imwrite(sim,sprintf('../Paper_revise1/img_sit_gt/%s.jpg',id));
%end
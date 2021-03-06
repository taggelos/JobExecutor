# JobExecutor
Utilize [MiniSearch](https://github.com/taggelos/MiniSearch) repository, using named pipes and signals.

<b><i> Arguments: </i></b> <br/>
./minisearch -d docfile -w number-of-workers  <br/>
./minisearch -d docfile (default value 10 gia to w)  <br/>
./minisearch -w number-of-workers -d docfile  <br/>

<b><i> Commands: </i></b> <br/>
<b>/search q1, q2 .. q10 –d deadline</b> (search 1 to 10 words and receive results inside the deadline period) <br/>
<b>/maxcount q1</b>                   {find the document with the highest frequency of the q1 (alphabetical order if 2 or more are the same)} <br/>
<b>/mincount q1</b>                {find the document with the lowest frequency of the q1 (at least one time)} <br/>
<b>/wc</b>                   (total number of bytes, words and lines from every file) <br/>

<b><i> Script: </i></b> <br/>
Using stats.sh we can use the below arguments to take these metrics<br/>
[-t] for total number of keywords searched <br/>
[-m] for most frequently keyword found <br/>
[-l] for least frequently keyword found <br/>
Or use no argument at all to do all of them in the same execution.

In the log folder that is automatically created we hold logs from each individual process (workers).

The architecture can be seen in the image below

![Image](images/architecture.png)

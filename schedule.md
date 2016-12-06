# Project Task List

### HTTP Server
- [x] basic multiprocess webserver (just responds to any request with dummy text)
- [ ] implement responses to requests like http://ip.address.of.server:port-number/request where `request` is
    * a subdirectory to list
    * an html file to view
    * a shell or perl script to execute
- [ ] respond with status codes:
    * 200 (success)
    * 404 (not found)
    * 501 (not implemented)
    * details on HTTP status codes
* **test cases**:
    - [ ] request for directory listing
    - [ ] request for valid html file
    - [ ] request for non-existent html file
    - [ ] request for static image file (gif or jpeg)
    - [ ] request for cgi script that requires execution of a bash shell command
    - [ ] request for a perl script or program of some language in cgi file for processing raw data and formatting it into an html file
    - [ ] request for a dynamically-created image using gnuplot


### my-histogram, Gnuplot, and html "pretty print"
- [ ] implement basic program `my-histogram dir` that searches all directories from `dir` and records the number of files of the types:
    * regular
    * directory
    * fifo
    * socket
    * symlink
    * block files
    * character files
- [ ] implement graph creating functionality by piping the collected data into gnuplot and:
    * show frequency on y-axis
    * show file types on x-axis
    * output the graph to file
    * have cgi script send image file to client
- [ ] "pretty print" the graph file by
    * embedding it in a generated html file
    * page style:
        * 16pt red centerd title: "CS410 Webserver"
        * white background
        * centered histogram image

#### Advanced Features (bonus)
* multi-threaded rather than multi-process server
* web cache
* server configuration options to turn off cache, threading

### Physical Computing
coming soon...

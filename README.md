The project uses autoconf and automake tools - so before building project need to install:
- autoconf
- automake
- install

To build project call in project directory
```
./run_configure.sh
```

Then install binaries by
```
make && make install
```

The result binary file is "install/bin/skareader".
After intalling it will be save only temporary for active session.
If you want to save it for global calling, save it in your PATH in .zshrc or .bashrc or .profile. 
```
export PATH="YOUR_PROJECT_DIR/install/bin:$PATH"
```
Or you can save it in your system directory /usr/local/bin by
```
make install-external
```

For using program to generate .txt file
```
skareader your_json_file.json
```
For using program to generate .html file
```
skareader -h your_json_file.json
```

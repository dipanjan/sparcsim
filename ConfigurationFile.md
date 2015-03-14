# _Configuration File_ #

---


SPARCSIM uses a configuration file sparcsim.conf to save its configuration data. It is a simple text file where entries are there in a form of name=value pair and separated by newline character from each other. Presently, it contains the number of REGISTER WINDOWS as its only entry. It is expected to be heavily used in future as and when the application develops. Usage of configuration file to store dynamic data makes the project scalable for future use. Also, it eliminates the need of recompilation if any such parameter is changed with time.
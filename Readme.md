# Musings about Memory :)

To create the nicely rendered AsciiDoctor output run:
```console
$ docker build -t asciidoctor-memory -f Dockerfile .
$ docker run -it -u `id -u`:`id -g` -v `pwd`:/workdir -w /workdir asciidoctor-memory
```

The generated pages will be served from [https://simonis.github.io/Memory/LinuxProcessLayout.html](https://simonis.github.io/Memory/LinuxProcessLayout.html) [https://simonis.github.io/Memory/Uncommit.html](https://simonis.github.io/Memory/Uncommit.html).

One day I'll change this into a real blog with proper CI/GithubActions integration...


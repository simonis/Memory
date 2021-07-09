FROM asciidoctor/docker-asciidoctor:1.7.0

ENTRYPOINT ["/usr/bin/asciidoctor", "-a", "rouge-style=github", "-a", "linkcss", "-a", "stylesheet=github_simonis.css", "-a", "stylesdir=./styles", "-a", "copycss=./styles/github_simonis.css", "--destination-dir", "docs/", "-r", "./ruby/callout-macro.rb", "--trace", "*.adoc"]

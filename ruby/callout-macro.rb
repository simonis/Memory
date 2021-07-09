require 'asciidoctor/extensions' unless RUBY_ENGINE == 'opal'

include Asciidoctor

# An callout macro that generates callouts in normal text sections
#
# Usage
#
#   callout:<number>[]
#
class CalloutMacro < Extensions::InlineMacroProcessor
  use_dsl

  named :callout

  def process parent, target, attrs
    if parent.document.basebackend? 'html'
      %(<i class="conum" data-value="#{target}"></i><b>#{target}</b>)
    else
      %(#{target})
    end
  end
end

Extensions.register do
  inline_macro CalloutMacro
end

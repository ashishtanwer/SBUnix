parse_git_branch () {
  git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ (\1)/'
}

parse_git_tag () {
  git describe --tags 2> /dev/null
}

parse_git_branch_or_tag() {
  local OUT="$(parse_git_branch)"
  if [ "$OUT" == " ((no branch))" ]; then
    OUT="($(parse_git_tag))";
  fi
  echo $OUT
}

# avoid duplicates..
export HISTCONTROL=ignoredups:erasedups

# append history entries..
export HISTSIZE=100000                   # big big history
export HISTFILESIZE=100000               # big big history

shopt -s histappend

# After each command, save and reload history
export PROMPT_COMMAND="history -a;  $PROMPT_COMMAND"
eval $(dircolors -b $HOME/.dircolors)
alias ls="ls --color=auto"
alias grep="grep --color=auto"


# If id command returns zero, you’ve root access.
if [ $(id -u) -eq 0 ];
then # you are root, set red colour prompt
  export PS1="\\[$(tput setaf 1)\\]\\u@\\h:\\w #\\[$(tput sgr0)\\]"
else # normal
  PS1="[\\u@\\h:\\w] $"
  export PS1="\[\e[0;32m\]\u\[\e[m\]\[\e[0;35m\]@`hostname -s`\[\e[m\] \[\e[1;34m\]\w\[\e[m\] \[\e[1;32m\]\$(parse_git_branch_or_tag)$\[\e[m\] \[\e[1;37m\] "
fi

# enable bash completion in interactive shells
if [ -f /etc/bash_completion ] && ! shopt -oq posix; then
    . /etc/bash_completion
fi

SCALA_HOME=/home/stufs1/jpudipeddi/scala-2.10.2
PATH=/home/facfs1/mferdman/cse506-tools/bin:"$PATH"
#export VIMRUNTIME=~/.vim

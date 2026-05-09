#!/data/data/com.termux/files/usr/bin/bash
V="$HOME/.vault_core"; S="$HOME/.vault_salt"
k() { echo "$(getprop ro.build.fingerprint)$(cat $S)" | sha256sum | awk '{print $1}'; }
case "$1" in
    stash) openssl enc -aes-256-cbc -salt -pbkdf2 -iter 100000 -k "$(k)" -in "$2" -out "$V/$(basename $2).sipv" && shred -u -n 4 "$2" ;;
    retrieve) openssl enc -d -aes-256-cbc -pbkdf2 -iter 100000 -k "$(k)" -in "$V/$(basename $2).sipv" -out "$HOME/$(basename $2 .sipv)" ;;
esac

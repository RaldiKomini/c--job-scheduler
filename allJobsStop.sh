stop() {
#-r gia ta \
    while read -r line
        do
            IFS=$'\t'
            set $line
            ./jobCommander stop "$1"
        done < <(./jobCommander poll "$1")
}

stop queued
stop running
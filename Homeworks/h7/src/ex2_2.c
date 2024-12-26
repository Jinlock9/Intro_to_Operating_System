#include <uapi/linux/ptrace.h>
#include <linux/sched.h>
#include <bcc/proto.h>

BPF_HASH(fork_count, u32, u64);

int on_sys_clone(struct pt_regs *ctx) {
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    u64 *count, zero = 0;

    count = fork_count.lookup_or_init(&pid, &zero);
    (*count)++;
    return 0;
}

int on_sys_clone_ret(struct pt_regs *ctx) {
    return 0;
}

#include <bcc/BPF.h>

int main() {
    BPF bpf;
    bpf.attach_kprobe("do_fork", "on_sys_clone");
    bpf.attach_kretprobe("do_fork", "on_sys_clone_ret");

    printf("Tracking subprocesses...\n");
    bpf.trace_print();
    return 0;
}
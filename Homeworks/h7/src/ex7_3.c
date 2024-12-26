#include <uapi/linux/ptrace.h>
#include <linux/sched.h>
#include <bcc/proto.h>

BPF_HASH(fork_count, u32, u64);

TRACEPOINT_PROBE(sched, sched_process_fork) {
    u32 parent_pid = args->parent_pid;
    u32 child_pid = args->child_pid;
    u64 *count, zero = 0;

    count = fork_count.lookup_or_init(&parent_pid, &zero);
    (*count)++;
    bpf_trace_printk("Parent PID: %d, Child PID: %d\n", parent_pid, child_pid);
    return 0;
}

#include <bcc/BPF.h>

int main() {
    BPF bpf;
    bpf.attach_tracepoint("sched:sched_process_fork", "sched_process_fork");

    printf("Tracking subprocesses using tracepoints...\n");
    bpf.trace_print();
    return 0;
}